cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
project(qTrader)
set(requiredlibs)

find_package(Threads REQUIRED)
set(requiredlibs ${requiredlibs} ${CMAKE_THREAD_LIBS_INIT})

find_package(rapidjson REQUIRED)
if(RAPIDJSON_FOUND)
  include_directories(${RAPIDJSON_INCLUDE_DIRS})
else(RAPIDJSON_FOUND)
  message(FATAL_ERROR "Could not find RapidJson Library.")
endif(RAPIDJSON_FOUND)


find_package(Crypto++ REQUIRED)
if(CRYPTO++_FOUND)
  include_directories(${CRYPTO++_INCLUDE_DIR})
  set(requiredlibs ${requiredlibs} ${CRYPTO++_LIBRARIES})
else(CRYPTO++_FOUND)
  message(FATAL_ERROR "Could not find Crypto++ library")
endif(CRYPTO++_FOUND)

find_package(CURL)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  set(requiredlibs ${requiredlibs} ${CURL_LIBRARIES})
else(CURL_FOUND)
  message(FATAL_ERROR "Could not find the CURL library and development files.")
endif(CURL_FOUND)

find_package(Casablanca)
include_directories(${CASABLANCA_INCLUDE_DIR})
set(requiredlibs ${requiredlibs} ${CASABLANCA_LIBRARIES})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (${CMAKE_CXX_COMPILER} MATCHES "icpc.*$")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -xHOST -unroll-aggressive -inline-level=2")
endif()

include_directories(include)
file(GLOB SOURCES "src/*.cpp")
add_executable(qTrader ${SOURCES})

target_link_libraries(qTrader ${requiredlibs})
