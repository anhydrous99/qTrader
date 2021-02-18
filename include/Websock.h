#include <cpprest/ws_client.h>
#include <vector>
#include <mutex>


class Websock {
private:
    web::websockets::client::websocket_callback_client client;

    void message_handler(const web::websockets::client::websocket_incoming_message &msg);

    void send_message(const std::string &to_send);

    std::vector<double> buy_prices;
    std::vector<double> sell_prices;

    inline std::string subscribe(bool sub);

    std::mutex buy_mut, sell_mut;

    std::vector<std::string> Channels;
    std::string Product_id;
    std::string Uri;
public:
    double Best_Buy_Price();

    double Best_Sell_Price();

    double MidMarket_Price();

    void Connect();

    void Connect(std::vector<std::string> channels, std::string product_id, std::string uri);

    void Disconnect();

    void Set_Channels(std::vector<std::string> channels);

    void Set_Product_id(std::string product_id);

    void Set_Uri(std::string uri);

    void Set_Data(std::vector<std::string> channels, std::string product_id, std::string uri);

    Websock(std::vector<std::string> channels, std::string product_id, std::string uri);

    Websock() = default;

    ~Websock();
};

