#include "Auth.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/hmac.h"
#include "cryptopp/sha.h"
#include "cryptopp/base64.h"
#include "cryptopp/filters.h"

using CryptoPP::Exception;
using CryptoPP::HMAC;
using CryptoPP::SHA256;
using CryptoPP::Base64Decoder;
using CryptoPP::Base64Encoder;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::HashFilter;

string Auth::Sign(string time_stamp, string method, string path, string body)
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
string Auth::Sign(string time_stamp, string method, string path)
{
  return Sign(time_stamp, method, path, "");
}
string Auth::GetTimestamp()
{
  time_t t = time(0);
  return to_string(t);
}
Auth::Auth(string key, string secret, string passphrase)
{ Key = key; Secret = secret; Passphrase = passphrase; }
