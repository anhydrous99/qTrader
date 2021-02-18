#include "Auth.h"
#include "Websock.h"

class API {
private:
    string Call(const string &method, bool authed, const string &path, const string &body);

    string Call(bool authed, const string &path);

    Websock sock;
public:
    API();

    ~API();

    Auth auth;
    int interval;
    int percent_transact;
    string uri;
    string socket_uri;
    string product_id;

    bool Is_Order_filled(const string &order_id);

    string Place_Limit_Order(const string &side, const string &price, const string &size);

    string Place_Market_Order(const string &side, const string &size, const string &funds);

    string Place_Stop_Order(const string &side, const string &price, const string &size, const string &funds);

    string Get_AccountID(const string &currency);

    string Get_AccountID(const string &currency, string &balance);

    string Get_Buy_Price();

    string Get_Sell_Price();

    double Get_MidMarketPrice();

    void Delete_Order(const string &order_id);

    void Delete_All_Orders();

    string Asset_Currency() const;

    string Fiat_Currency() const;

    double Get_Balance(const string &currency);

    void Get_Balances(double &fiat_balance, double &asset_balance);

    void Show_Balances();

    void Insta_Limit_Buy();

    void Insta_Limit_Sell();

    void Insta_Market_Sell();

    void Insta_Market_Buy();
};
