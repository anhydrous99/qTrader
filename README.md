# qTrader
### Build Ubuntu

```
sudo apt update
sudo apt install build-essential git cmake rapidjson-dev libcrypto++-dev libcurl4-openssl-dev libcpprest-dev
git clone https://github.com/anhydrous99/qTrader
mkdir qTrader/build && cd qTrader/build
cmake ..
make
```
