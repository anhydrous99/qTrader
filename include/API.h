#include "Auth.h"
#include "Websock.h"

class API
{
private:
  string Call(string method, bool authed, string path, string body);
  string Call(bool authed, string path);
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
  bool Is_Order_filled(string order_id);
  string Place_Limit_Order(string side, string price, string size);
  string Place_Market_Order(string side, string size, string funds);
  string Place_Stop_Order(string side, string price, string size, string funds);
  string Get_AccountID(string currency);
  string Get_AccountID(string currency, string &balance);
  string Get_Buy_Price();
  string Get_Sell_Price();
  double Get_MidMarketPrice();
  void Delete_Order(string order_id);
  void Delete_All_Orders();
  string Asset_Currency();
  string Fiat_Currency();
  double Get_Balance(string currency);
  void Get_Balances(double& fiat_balance, double& asset_balance);
  void Show_Balances();
  void Insta_Limit_Buy();
  void Insta_Limit_Sell();
  void Insta_Market_Sell();
  void Insta_Market_Buy();
};
