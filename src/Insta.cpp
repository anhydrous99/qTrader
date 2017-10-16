#include "API.h"
#include <iomanip>
#include <cmath>
#include <chrono>
#include <future>

const string st_black = "\033[30m";
const string st_red = "\033[31m";
const string st_green = "\033[32m";
const string st_yellow = "\033[33m";
const string st_blue = "\033[34m";
const string st_magenta = "\033[35m";
const string st_cyan = "\033[36m";
const string st_white = "\033[37m";
const string st_reset = "\033[0m";
const string st_space = " ";
string to_bold(string st_color)
{ return st_color.insert(2, "1;"); }

void API::Insta_Limit_Sell()
{  
  string currency = Asset_Currency();
  string price = to_string(sock.Best_Sell_Price());
  double b_balance = Get_Balance(currency) * percent_transact / 100;
  if (b_balance < 0.01)
  {
    cout << "\033[1;31mOrder size is too small. Minimum size is 0.01\033[0m\n";
    cout << "Your balance: " << to_bold(st_green) << fixed << setprecision(8) << 
            (b_balance * 100) / percent_transact << st_reset << " " << Asset_Currency() << endl;
    return;
  }
  string order_id = Place_Limit_Order("sell", price, to_string(b_balance));
  cout << "Placed order to sell " << to_bold(st_green) << fixed << setprecision(8) << b_balance << 
  st_reset << " " << Asset_Currency() << " at " << to_bold(st_green) << fixed << setprecision(2) << 
  stod(price) << st_reset << " " << Fiat_Currency() << endl;
 
  bool stop = false;
  while (!stop)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    stop = Is_Order_filled(order_id);
    if (stop)
      break;
    string new_price = to_string(sock.Best_Sell_Price());
    if (new_price != price)
    {
      price = new_price;
      Delete_Order(order_id);
      order_id = Place_Limit_Order("sell", price, to_string(b_balance));
      cout << "New Price: " << to_bold(st_green) << fixed << setprecision(2) << stod(new_price) <<
      st_reset << " " << Fiat_Currency() << endl;
    }
  }
  cout << to_bold(st_cyan) << "Successful Limit Sell" << st_reset << endl;
}
void API::Insta_Limit_Buy()
{
  string currency = Fiat_Currency();
  string price = to_string(sock.Best_Buy_Price());
  double fiat_bal = Get_Balance(currency);
  double b_balance = (fiat_bal * percent_transact) / (100 * stod(price));
  if (b_balance < 0.01)
  {
    cout << to_bold(st_red) << "Order size is too small. Minimum size is 0.01" << st_reset << endl;
    cout << "Your order size: " << to_bold(st_green) << fixed << setprecision(8) << 
            (b_balance * 100) / percent_transact << st_reset << " " << Asset_Currency() << endl;
	return;
  }
  string order_id = Place_Limit_Order("buy", price, to_string(b_balance));
  
  cout << "Placed order to buy " << to_bold(st_green) << fixed << setprecision(8) << b_balance <<
  st_reset << " " << Asset_Currency() << " at " << to_bold(st_green) << fixed << setprecision(2) << 
  stod(price) << st_reset << " " << Fiat_Currency() << endl; 
  
  bool stop = false;
  while(!stop)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    stop = Is_Order_filled(order_id);
    if (stop)
      break;
    string new_price = to_string(sock.Best_Buy_Price());
    if (new_price != price)
    {
      price = new_price;
      Delete_Order(order_id);
      b_balance = (fiat_bal * percent_transact) / (100 * stod(price));
      order_id = Place_Limit_Order("buy", price, to_string(b_balance));
      cout << "New Price: " << to_bold(st_green) << fixed << setprecision(2) << stod(new_price) <<
      st_reset << " " << Fiat_Currency() << " for " << to_bold(st_green) << fixed << setprecision(8) << 
      b_balance << st_reset << " " << Asset_Currency() << endl;
    }
  }
  cout << to_bold(st_cyan) << "Successful Limit Buy" << st_reset << endl;
}
void API::Insta_Market_Sell()
{
  string currency = Asset_Currency();
  double balance = Get_Balance(currency);
  string order_id = Place_Market_Order("sell", "", to_string(balance));
  cout << "Placed Market Sell of " << to_bold(st_green) << fixed << setprecision(8) << balance << st_reset <<
          " " << Asset_Currency() << endl;
}
void API::Insta_Market_Buy()
{
  string currency = Fiat_Currency();
  double balance = Get_Balance(currency);
  string order_id = Place_Market_Order("buy", to_string(balance), "");
  cout << "Placed Market Buy of " << to_bold(st_green) << fixed << setprecision(2) << balance << st_reset <<
          " " << Fiat_Currency() << endl;
}
void API::Show_Balances()
{ 
  double fiat_balance, asset_balance;
  string fiat_curr = Fiat_Currency(), asset_curr = Asset_Currency();
  Get_Balances(fiat_balance, asset_balance);
  cout << "Balance:       " << to_bold(st_green) << fixed << setprecision(2) << fiat_balance << 
          st_reset << " " << fiat_curr << endl;
  cout << "               " << to_bold(st_green) << fixed << setprecision(8) << asset_balance << 
          st_reset << " " << asset_curr << endl;
  double midmark = sock.MidMarket_Price();
  cout << "Current Price: " << to_bold(st_cyan) << fixed << setprecision(3) << midmark << st_reset << 
          " " << fiat_curr << endl;
  cout << "Asset Price:   " << to_bold(st_yellow) << fixed << setprecision(2) << (midmark * asset_balance) <<
          st_reset << " " << fiat_curr << endl;
  cout << "Total:         " << to_bold(st_green) << fixed << setprecision(2) << 
          (midmark * asset_balance) + fiat_balance << st_reset << " " << fiat_curr << endl;
  cout << "Total:         " << to_bold(st_green) << fixed << setprecision(8) <<
          (fiat_balance / midmark) + asset_balance << st_reset << " " << asset_curr << endl;
}
