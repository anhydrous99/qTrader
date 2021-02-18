#include <iostream>
#include <string>

using namespace std;

class Auth
{
public:
  string Key;
  string Secret;
  string Passphrase;
  string Sign(string time_stamp, string method, string path, string body);
  string Sign(string time_stamp, string method, string path);
  string GetTimestamp();

  Auth() = default;
  Auth(string key, string secret, string passphrase);
};
