cmake_minimum_required(VERSION 2.8)

project(RestPP)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_subdirectory(3rd/gtest-1.7.0)
include_directories(3rd/asio/asio/include)

add_subdirectory(src)
add_subdirectory(tests)
