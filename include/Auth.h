#include <iostream>
#include <string>

class Auth {
public:
    std::string Key;
    std::string Secret;
    std::string Passphrase;

    std::string Sign(std::string time_stamp, std::string method, std::string path, std::string body);

    std::string Sign(std::string time_stamp, std::string method, std::string path);

    std::string GetTimestamp();

    Auth() = default;

    Auth(std::string key, std::string secret, std::string passphrase);
};
