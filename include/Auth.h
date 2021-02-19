#include <iostream>
#include <string>

class Auth {
public:
    std::string Key;
    std::string Secret;
    std::string Passphrase;

    std::string Sign(const std::string& time_stamp, const std::string& method, const std::string& path, const std::string& body) const;

    std::string Sign(const std::string& time_stamp, const std::string& method, const std::string& path) const;

    static std::string GetTimestamp();

    Auth() = default;

    Auth(std::string key, std::string secret, std::string passphrase);
};
