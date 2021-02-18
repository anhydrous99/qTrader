#include "Websock.h"
#include <mutex>
#include <iostream>
#include <algorithm>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

Websock::Websock(std::vector<std::string> channels, std::string product_id, std::string uri)
{
  Channels = channels; Product_id = product_id; Uri = uri;
  Connect();
}
Websock::~Websock()
{
  std::string unsub = subscribe(false);
  send_message(unsub);
  client.close().wait();
}
void Websock::send_message(std::string to_send)
{
  web::websockets::client::websocket_outgoing_message out_msg;
  out_msg.set_utf8_message(to_send);
  client.send(out_msg).wait();
}
void Websock::message_handler(web::websockets::client::websocket_incoming_message msg)
{
  std::string input = msg.extract_string().get();
  Document d;
  d.Parse(input.c_str()); 
  assert(d.HasMember("type"));
  assert(d["type"].IsString());
  std::string type = d["type"].GetString();
  if (type == "snapshot")
  {
    const Value& bids = d["bids"];
    const Value& asks = d["asks"];
    assert(bids.IsArray());
    assert(asks.IsArray());
    //std::scoped_lock<std::mutex, std::mutex> lock{buy_mut, sell_mut};
    std::lock_guard<std::mutex> buy_lock(buy_mut);
    std::lock_guard<std::mutex> sell_lock(sell_mut);
    buy_prices.reserve(bids.Size());
    sell_prices.reserve(asks.Size());
    for (SizeType i = 0; i < bids.Size(); i++)
    {
      assert(bids[i].IsArray());
      assert(bids[i][0].IsString());
      buy_prices.push_back(std::stod(bids[i][0].GetString()));
    }
    for (SizeType i = 0; i < asks.Size(); i++)
    {
      assert(asks[i].IsArray());
      assert(asks[i][0].IsString());
      sell_prices.push_back(std::stod(asks[i][0].GetString()));
    }
  }
  else if (type == "l2update")
  {
    const Value& changes = d["changes"];
    assert(changes.IsArray());
    //std::scoped_lock lock{buy_mut, sell_mut};
    std::lock_guard<std::mutex> buy_lock(buy_mut);
    std::lock_guard<std::mutex> sell_lock(sell_mut);
    for (SizeType i = 0; i < changes.Size(); i++)
    {
      assert(changes[i].IsArray());
      assert(changes[i][0].IsString());
      assert(changes[i][1].IsString());
      double price = std::stod(changes[i][1].GetString());
      std::string side = changes[i][0].GetString();
      if (side == "buy")
      {
        assert(changes[i][2].IsString());
        if (0 == std::stod(changes[i][2].GetString()))
          buy_prices.erase(std::remove(buy_prices.begin(), buy_prices.end(), price), buy_prices.end());
        else
          buy_prices.push_back(price);
      }
      else
      {
        assert(changes[i][2].IsString());
        if (0 == std::stod(changes[i][2].GetString()))
          sell_prices.erase(std::remove(sell_prices.begin(), sell_prices.end(), price), sell_prices.end());
        else
          sell_prices.push_back(price);
      }
    }
  }
}
double Websock::Best_Buy_Price()
{
  std::lock_guard<std::mutex> lock(buy_mut);
  auto biggest = std::max_element(std::begin(buy_prices), std::end(buy_prices));
  return *biggest; 
}
double Websock::Best_Sell_Price()
{
  std::lock_guard<std::mutex> lock(sell_mut);
  auto smallest = std::min_element(std::begin(sell_prices), std::end(sell_prices)); 
  return *smallest;
}
double Websock::MidMarket_Price()
{
  return (Best_Buy_Price() + Best_Sell_Price()) / 2;
}
inline std::string Websock::subscribe(bool sub)
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
  if (sub)
    d.AddMember("type", "subscribe", allocator);
  else
    d.AddMember("type", "unsubscribe", allocator);
  Value product_ids(kArrayType);
  product_ids.PushBack(Value().SetString(StringRef(Product_id.c_str())), allocator);
  d.AddMember("product_ids", product_ids, allocator);
  Value channels(kArrayType);
  for (std::string& channel : Channels)
    channels.PushBack(Value().SetString(StringRef(channel.c_str())), allocator);
  d.AddMember("channels", channels, allocator);
  /* create string with rapidjson */
  StringBuffer strbuf;
  Writer<StringBuffer> writer(strbuf);
  d.Accept(writer);
  return strbuf.GetString(); 
}
void Websock::Connect()
{
  client.set_message_handler([this](web::websockets::client::websocket_incoming_message msg){message_handler(msg);});
#ifdef _WIN32
  client.connect(web::uri(utility::conversions::to_string_t(Uri))).wait();
#else
  client.connect(Uri).wait();
#endif
  std::string sub = subscribe(true);
  send_message(sub);
}
void Websock::Connect(std::vector<std::string> channels, std::string product_id, std::string uri)
{
  Channels = channels; Product_id = product_id; Uri = uri;
  Connect();
}
void Websock::Disconnect()
{
  std::string unsub = subscribe(false);
  send_message(unsub);
  client.close().wait();
}
void Websock::Set_Channels(std::vector<std::string> channels) { Channels = channels; }
void Websock::Set_Product_id(std::string product_id) { Product_id = product_id; }
void Websock::Set_Uri(std::string uri) { Uri = uri; }
void Websock::Set_Data(std::vector<std::string> channels, std::string product_id, std::string uri)
{
  Channels = channels;
  Product_id = product_id;
  Uri = uri;
}
