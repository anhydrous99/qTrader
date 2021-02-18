#include "API.h"
#include <fstream>
#include <sys/stat.h>
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

/* Used by API::Call to put websource into a string type */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((string *) userp)->append((char *) contents, size * nmemb);
    return size * nmemb;
}

/* Uses LibCurl to get Data from API */
string API::Call(const string &method, bool authed, const string &path, const string &body) {
    CURL *curl;
    CURLcode res;
    string readBuffer;
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *chunk = nullptr;
        curl_easy_setopt(curl, CURLOPT_URL, (uri + path).c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl/1.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        chunk = curl_slist_append(chunk, "Content-Type: application/json");
        if (authed) {
            string time_stamp = auth.GetTimestamp();
            string sign = auth.Sign(time_stamp, method, path, body);
            chunk = curl_slist_append(chunk, ("CB-ACCESS-KEY: " + auth.Key).c_str());
            chunk = curl_slist_append(chunk, ("CB-ACCESS-SIGN: " + sign).c_str());
            chunk = curl_slist_append(chunk, ("CB-ACCESS-TIMESTAMP: " + time_stamp).c_str());
            chunk = curl_slist_append(chunk, ("CB-ACCESS-PASSPHRASE: " + auth.Passphrase).c_str());
        }
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            printf("curl_easy_perform() failed: %s\n",
                   curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
        /* free the custom headers */
        curl_slist_free_all(chunk);
    }
    return readBuffer;
}

string API::Call(bool authed, const string &path) {
    return Call("GET", authed, path, "");
}

/* Checks if an Order is filled */
bool API::Is_Order_filled(const string &order_id) {
    string returned_str = Call(true, "/orders/" + order_id);
    Document d;
    d.Parse(returned_str.c_str());
    if (d.IsObject()) {
        if (d.HasMember("message")) {
            assert(d["message"].IsString());
            cerr << "(Is_Order_filled) Error" << d["message"].GetString() << endl;
        }
    }
    if (d.IsArray() && d.Size() != 0 && d[0].HasMember("status")) {
        assert(d[0]["status"].IsString());
        string status = d[0]["status"].GetString();
        if (status == "done" || status == "settled")
            return true;
    } else if (d.IsArray() && d.Size() == 0)
        return true;

    return false;
}

string API::Place_Limit_Order(const string &side, const string &price, const string &size) {
    string order_id;
    Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

    /* adds type */
    d.AddMember("type", "limit", allocator);

    /* Adds the Side */
    Value v_side;
    v_side = StringRef(side.c_str());
    d.AddMember("side", v_side, allocator);

    /* Adds the Product id */
    Value v_product_id;
    v_product_id = StringRef(product_id.c_str());
    d.AddMember("product_id", v_product_id, allocator);

    /* Adds the price (USD) */
    Value v_price;
    v_price = StringRef(price.c_str());
    d.AddMember("price", v_price, allocator);

    /* Adds the size (BTC, ETH, LTC) */
    Value v_size;
    v_size = StringRef(size.c_str());
    d.AddMember("size", v_size, allocator);

    /* Gets the Order to be a maker and not a taker */
    d.AddMember("post_only", true, allocator);

    /* creates string with json */
    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);
    d.Accept(writer);
    string returned = Call("POST", true, "/orders", strbuf.GetString());
    Document d_1;
    d_1.Parse(returned.c_str());
    if (d_1.HasMember("order_id")) {
        assert(d_1["order_id"].IsString());
        order_id = d_1["order_id"].GetString();
    }
    if (d_1.HasMember("message")) {
        assert(d_1["message"].IsString());
        printf("(Limit_Order) Message: %s\n", d_1["message"].GetString());
    }
    return order_id;
}

string API::Place_Market_Order(const string &side, const string &size, const string &funds) {
    string order_id;
    Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

    /* adds type */
    d.AddMember("type", "market", allocator);

    /* Adds the Side */
    Value v_side;
    v_side = StringRef(side.c_str());
    d.AddMember("side", v_side, allocator);

    /* Adds the Product id */
    Value v_product_id;
    v_product_id = StringRef(product_id.c_str());
    d.AddMember("product_id", v_product_id, allocator);

    /* Adds size or funds depending on which is null */
    bool size_empty = size.empty();
    bool funds_empty = funds.empty();
    if (size_empty ^ funds_empty) {
        if (!size_empty) {
            Value v_size;
            v_size = StringRef(size.c_str());
            d.AddMember("size", v_size, allocator);
        } else if (!funds_empty) {
            Value v_funds;
            v_funds = StringRef(funds.c_str());
            d.AddMember("funds", v_funds, allocator);
        }
    } else {
        printf("\033[31mError:\033[0m Either both size and funds are empty or filled.\n");
        return "";
    }

    /* Creates string with rapidjson */
    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);
    d.Accept(writer);
    string returned = Call("POST", true, "/orders", strbuf.GetString());
    Document d_1;
    d_1.Parse(returned.c_str());
    if (d_1.HasMember("order_id")) {
        assert(d_1["order_id"].IsString());
        order_id = d_1["order_id"].GetString();
    }
    if (d_1.HasMember("message")) {
        assert(d_1["message"].IsString());
        printf("(Market_Order) Message: %s\n", d_1["message"].GetString());
    }
    return order_id;
}

string API::Place_Stop_Order(const string &side, const string &price, const string &size, const string &funds) {
    string order_id;
    Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType &allocator = d.GetAllocator();

    /* adds type */
    d.AddMember("type", "stop", allocator);

    /* Adds the Side */
    Value v_side;
    v_side = StringRef(side.c_str());
    d.AddMember("side", v_side, allocator);

    /* Adds the Product id */
    Value v_product_id;
    v_product_id = StringRef(product_id.c_str());
    d.AddMember("product_id", v_product_id, allocator);

    /* Adds the price */
    Value v_price;
    v_price = StringRef(price.c_str());
    d.AddMember("price", v_price, allocator);

    /* Adds size or funds depending on which is null */
    bool size_empty = size.empty();
    bool funds_empty = funds.empty();
    if (size_empty ^ funds_empty) {
        if (!size_empty) {
            Value v_size;
            v_size = StringRef(size.c_str());
            d.AddMember("size", v_size, allocator);
        } else if (!funds_empty) {
            Value v_funds;
            v_funds = StringRef(funds.c_str());
            d.AddMember("funds", v_funds, allocator);
        }
    } else {
        printf("\033[31mError:\033[0m Either both size and funds are empty or filled.\n");
        return "";
    }

    /* Creates string with rapidjson */
    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);
    d.Accept(writer);
    string returned = Call("POST", true, "/orders", strbuf.GetString());
    Document d_1;
    d_1.Parse(returned.c_str());
    if (d_1.HasMember("order_id")) {
        assert(d_1["order_id"].IsString());
        order_id = d_1["order_id"].GetString();
    }
    if (d_1.HasMember("message")) {
        assert(d_1["message"].IsString());
        printf("(Stop_Order) Message: %s\n", d_1["message"].GetString());
    }
    return order_id;
}

string API::Get_AccountID(const string &currency) {
    string ret;
    string st = Call(true, "/accounts");
    Document d;
    d.Parse(st.c_str());
    if (d.HasMember("message")) {
        assert(d["message"].IsString());
        printf("(Get_AccountID) Message: %s\n", d["message"].GetString());
        return "";
    }
    assert(d.IsArray());
    Value &b = d;
    for (SizeType i = 0; i < b.Size(); i++) {
        assert(d[i]["currency"].IsString());
        if (d[i]["currency"].GetString() == currency) {
            assert(d[i]["id"].IsString());
            ret = d[i]["id"].GetString();
            break;
        }
    }
    return ret;
}

string API::Get_AccountID(const string &currency, string &balance) {
    string ret;
    string st = Call(true, "/accounts");
    Document d;
    d.Parse(st.c_str());
    if (d.HasMember("message")) {
        assert(d["message"].IsString());
        printf("(Get_AccountID) Message: %s\n", d["message"].GetString());
        return "";
    }
    assert(d.IsArray());
    Value &b = d;
    for (SizeType i = 0; i < b.Size(); i++) {
        assert(d[i]["currency"].IsString());
        if (d[i]["currency"].GetString() == currency) {
            assert(d[i]["available"].IsString());
            string avai = d[i]["available"].GetString();
            balance = avai;
            assert(d[i]["id"].IsString());
            ret = d[i]["id"].GetString();
            break;
        }
    }
    return ret;
}

string API::Get_Buy_Price() {
    string ret;
    string st = Call(false, "/products/" + product_id + "/book");
    Document d;
    d.Parse(st.c_str());
    if (d.HasMember("message")) {
        assert(d["message"].IsString());
        printf("(Get_Buy_Price) Message: %s\n", d["message"].GetString());
    }
    if (d.HasMember("bids")) {
        assert(d["bids"].IsArray());
        const Value &c = d["bids"];
        assert(c[0].IsArray());
        const Value &b = c[0];
        assert(b[0].IsString());
        ret = b[0].GetString();
    }
    return ret;
}

string API::Get_Sell_Price() {
    string ret;
    string st = Call(false, "/products/" + product_id + "/book");
    Document d;
    d.Parse(st.c_str());
    if (d.HasMember("message")) {
        assert(d["message"].IsString());
        printf("(Get_Sell_Price) Message: %s\n", d["message"].GetString());
        return "";
    }
    if (d.HasMember("asks")) {
        assert(d["asks"].IsArray());
        const Value &c = d["asks"];
        assert(c[0].IsArray());
        const Value &b = c[0];
        assert(b[0].IsString());
        ret = b[0].GetString();
    }
    return ret;
}

double API::Get_MidMarketPrice() {
    double ret = 0;
    string st = Call(false, "/products/" + product_id + "/book");
    Document d;
    d.Parse(st.c_str());
    if (d.HasMember("message")) {
        assert(d["message"].IsString());
        printf("(Get_MidMarketPrice) Message: %s\n", d["message"].GetString());
        return 0;
    }
    if (d.HasMember("bids") && d.HasMember("asks")) {
        assert(d["bids"].IsArray());
        assert(d["asks"].IsArray());
        const Value &c_a = d["bids"];
        const Value &c_b = d["asks"];
        assert(c_a[0].IsArray());
        assert(c_b[0].IsArray());
        const Value &a_a = c_a[0];
        const Value &a_b = c_b[0];
        assert(a_a[0].IsString());
        assert(a_b[0].IsString());
        double dbl_1 = stod(a_a[0].GetString());
        double dbl_2 = stod(a_b[0].GetString());
        ret = (dbl_1 + dbl_2) / 2;
    }
    return ret;
}

void API::Delete_Order(const string &order_id) {
    Call("DELETE", true, "/orders/" + order_id, "");
}

void API::Delete_All_Orders() {
    Call("DELETE", true, "/orders", "");
}

string API::Asset_Currency() const {
    return product_id.substr(0, 3);
}

string API::Fiat_Currency() const {
    return product_id.substr(4, 3);
}

double API::Get_Balance(const string &currency) {
    double ret = 0;
    string txt = Call(true, "/accounts");
    Document d;
    d.Parse(txt.c_str());
    assert(d.IsArray());
    for (SizeType i = 0; i < d.Size(); i++) {
        assert(d[i].HasMember("currency"));
        assert(d[i]["currency"].IsString());
        string cur = d[i]["currency"].GetString();
        if (cur == currency) {
            assert(d[i].HasMember("available"));
            assert(d[i]["available"].IsString());
            ret = stod(d[i]["available"].GetString());
            return ret;
        }
    }
    return ret;
}

void API::Get_Balances(double &fiat_balance, double &asset_balance) {
    double fiat_ret = 0, asset_ret = 0;
    string txt = Call(true, "/accounts");
    Document d;
    d.Parse(txt.c_str());
    assert(d.IsArray());
    for (SizeType i = 0; i < d.Size(); i++) {
        assert(d[i].HasMember("currency"));
        assert(d[i]["currency"].IsString());
        string cur = d[i]["currency"].GetString();
        if (cur == Fiat_Currency()) {
            assert(d[i].HasMember("available"));
            assert(d[i]["available"].IsString());
            fiat_ret = stod(d[i]["available"].GetString());
        } else if (cur == Asset_Currency()) {
            assert(d[i].HasMember("available"));
            assert(d[i]["available"].IsString());
            asset_ret = stod(d[i]["available"].GetString());
        }
    }
    fiat_balance = fiat_ret;
    asset_balance = asset_ret;
}

// Constructor and Destructor Stuff
bool exists(const string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void Generate_Ex_Conf() {
    Document d;
    d.SetObject();
    rapidjson::Document::AllocatorType &allocator = d.GetAllocator();
    /* Adds uri */
    d.AddMember("uri", "https://api.gdax.com", allocator);
    /* add socket_uri */
    d.AddMember("socket_uri", "wss://ws-feed.gdax.com", allocator);
    /* Adds product_id example */
    d.AddMember("product_id", "BTC-USD", allocator);
    /* Adds reaction_interval
         Interval between checks for new price
         when insta buying or selling */
    d.AddMember("reaction_interval", 100, allocator);
    /* Adds percent_transact */
    d.AddMember("percent_transact", 99, allocator);
    /* Adds the API Key area */
    d.AddMember("Key", "place-key-here", allocator);
    /* Adds the Secret area */
    d.AddMember("Secret", "place-secret-here", allocator);
    /* Adds the Passphrase area */
    d.AddMember("Passphrase", "place-pass-here", allocator);
    /* Creates string with prettywriter */
    StringBuffer strbuf;
    PrettyWriter<StringBuffer> writer(strbuf);
    d.Accept(writer);
    string json = strbuf.GetString();
    ofstream file;
    file.open("conf.json");
    file << json;
    file.close();
    printf("\033[31mError:\033[0m conf.json not found or was missing a string.\n");
    printf("       An example conf.json has been generated.\n");
    exit(EXIT_FAILURE);
}

void Get_Conf(string &uri, string &socket_uri, string &product_id, int &interval, int &perc_trans, Auth &auth) {
    ifstream file("conf.json");
    string str;
    string file_contents;
    while (getline(file, str)) {
        file_contents += str;
        file_contents.push_back('\n');
    }
    Document d;
    d.Parse(file_contents.c_str());
    bool h_0 = d.HasMember("uri");
    bool h_1 = d.HasMember("socket_uri");
    bool h_2 = d.HasMember("product_id");
    bool h_3 = d.HasMember("reaction_interval");
    bool h_4 = d.HasMember("percent_transact");
    bool h_5 = d.HasMember("Key");
    bool h_6 = d.HasMember("Secret");
    bool h_7 = d.HasMember("Passphrase");

    if (!(h_0 & h_1 & h_2 & h_3 & h_4 & h_5 & h_6 & h_7))
        Generate_Ex_Conf();

    assert(d["uri"].IsString());
    assert(d["socket_uri"].IsString());
    assert(d["product_id"].IsString());
    assert(d["reaction_interval"].IsInt());
    assert(d["percent_transact"].IsInt());
    assert(d["Key"].IsString());
    assert(d["Secret"].IsString());
    assert(d["Passphrase"].IsString());

    uri = d["uri"].GetString();
    socket_uri = d["socket_uri"].GetString();
    product_id = d["product_id"].GetString();
    interval = d["reaction_interval"].GetInt();
    perc_trans = d["percent_transact"].GetInt();
    if (!((0 < perc_trans) & (perc_trans <= 100))) {
        printf("\033[31mError:\033[0m percent_transact in conf.json can only be between 0 and 100.\n");
        exit(EXIT_FAILURE);
    }
    Auth tmp(d["Key"].GetString(), d["Secret"].GetString(), d["Passphrase"].GetString());
    auth = tmp;
}

API::API() : interval(0), percent_transact(0) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    string file = "conf.json";
    bool ex = exists(file);

    if (!ex)
        Generate_Ex_Conf();

    Get_Conf(uri, socket_uri, product_id, interval, percent_transact, auth);

    vector<string> channels = {"heartbeat", "level2"};
    sock.Connect(channels, product_id, socket_uri);
}

API::~API() {
    curl_global_cleanup();
}
