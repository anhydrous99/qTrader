#include "Auth.h"

#include <utility>

using namespace std;

string Auth::Sign(const string& time_stamp, const string& method, const string& path, const string& body)
{
  string mac, encoded, key;
  string plain = time_stamp + method + path + body;
  StringSource(Secret, true,
		  new Base64Decoder(
			  new StringSink(key)
			  ) // Base64Decoder
	      ); // StringSource
  try
  {
    HMAC< SHA256 > hmac((unsigned char*)key.c_str(), key.length());
    StringSource(plain, true,
		    new HashFilter(hmac,
			    new StringSink(mac)
			    ) // HashFilter
		); // StringSource
  }
  catch(const CryptoPP::Exception& e)
  {
    printf("\033[1;31m%s\033[0m\n", e.what());
  }
  encoded.clear();
  StringSource(mac, true,
		  new Base64Encoder(
			  new StringSink(encoded)
			  ) // Base64Encoder
	      ); // StringSource
  encoded.erase(44, 1);
  return encoded;
}
string Auth::Sign(const string& time_stamp, const string& method, const string& path)
{
  return Sign(time_stamp, method, path, "");
}
string Auth::GetTimestamp()
{
  time_t t = time(0);
  return to_string(t);
}
Auth::Auth(string key, string secret, string passphrase)
{ Key = std::move(key); Secret = std::move(secret); Passphrase = std::move(passphrase); }
