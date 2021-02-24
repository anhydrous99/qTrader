cmake_minimum_required(VERSION 3.17.5)

include("cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/anhydrous99/hunter/archive/v0.23.295-arm1.tar.gz"
        SHA1 "8fef8cbdc468b92a2a83d926414056451723dfaa"
)

project(qTrader)

hunter_add_package(RapidJSON)
hunter_add_package(CURL)
hunter_add_package(cryptopp)
hunter_add_package(cpprestsdk)

find_package(RapidJSON CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(cpprestsdk CONFIG REQUIRED)

# Required by cpprestsdk
find_package(Boost CONFIG REQUIRED system random thread filesystem chrono atomic date_time regex)
find_package(websocketpp CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(qTrader ${SOURCES})

target_link_libraries(qTrader CURL::libcurl cryptopp-static cpprestsdk::cpprest)
