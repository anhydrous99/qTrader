cmake_minimum_required(VERSION 3.17.5)

include("cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.294.tar.gz"
        SHA1 "0dd1ee8723d54a15822519c17a877c1f281fce39"
)

project(qTrader)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

hunter_add_package(RapidJSON)
hunter_add_package(CURL)
hunter_add_package(cryptopp)

find_package(RapidJSON CONFIG REQUIRED)
find_package(cryptopp CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Casablanca)

set(CMAKE_CXX_STANDARD 11)
include_directories(${CASABLANCA_INCLUDE_DIR})
include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(qTrader ${SOURCES})

target_link_libraries(qTrader CURL::libcurl OpenSSL::SSL OpenSSL::Crypto cryptopp-static ${CASABLANCA_LIBRARIES})
