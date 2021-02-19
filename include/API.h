#include "Auth.h"
#include "Websock.h"

class API {
private:
    std::string Call(const std::string &method, bool authed, const std::string &path, const std::string &body);

    std::string Call(bool authed, const std::string &path);

    Websock sock;
public:
    API();

    ~API();

    Auth auth;
    int interval;
    int percent_transact;
    std::string uri;
    std::string socket_uri;
    std::string product_id;

    bool Is_Order_filled(const std::string &order_id);

    std::string Place_Limit_Order(const std::string &side, const std::string &price, const std::string &size);

    std::string Place_Market_Order(const std::string &side, const std::string &size, const std::string &funds);

    std::string Place_Stop_Order(const std::string &side, const std::string &price, const std::string &size, const std::string &funds);

    std::string Get_AccountID(const std::string &currency);

    std::string Get_AccountID(const std::string &currency, std::string &balance);

    std::string Get_Buy_Price();

    std::string Get_Sell_Price();

    double Get_MidMarketPrice();

    void Delete_Order(const std::string &order_id);

    void Delete_All_Orders();

    std::string Asset_Currency() const;

    std::string Fiat_Currency() const;

    double Get_Balance(const std::string &currency);

    void Get_Balances(double &fiat_balance, double &asset_balance);

    void Show_Balances();

    void Insta_Limit_Buy();

    void Insta_Limit_Sell();

    void Insta_Market_Sell();

    void Insta_Market_Buy();
};
