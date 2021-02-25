# qTrader

This project has become too cumbersome and lacks tests. I'm starting over. Look for the better project down the line.

### Build Ubuntu

```
sudo apt update
sudo apt install build-essential git cmake rapidjson-dev libcrypto++-dev libcurl4-openssl-dev libcpprest-dev -y
git clone https://github.com/anhydrous99/qTrader
mkdir qTrader/build && cd qTrader/build
cmake ..
make
```
