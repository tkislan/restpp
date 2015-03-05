cmake_minimum_required(VERSION 2.8)

project(RestPP)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
add_definitions(-DASIO_STANDALONE=1)

find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

#add_subdirectory(3rd/gtest-1.7.0)
add_subdirectory(3rd/gmock-1.7.0)
include_directories(3rd/asio/asio/include)

add_subdirectory(src)
add_subdirectory(tests)
