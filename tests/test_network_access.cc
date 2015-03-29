#include "gtest/gtest.h"

#include "network_access.h"

using namespace restpp;

TEST(TestNetworkAccess, Simple) {
  NetworkAccess na;

//  Request req;
//  req.set_host("localhost");
//  req.set_port(8000);

//  req.set_method(HttpMethod::GET);
//  req.set_path("/api");
//  req.add_header("Host", "localhost:8000");
//  req.add_header("Connection", "close");

//  na.Run(std::move(req), [](const std::error_code &error, Response resp) {
//    if (error) {
//      std::cout << "Request failed: " << error.message() << std::endl;
//      return;
//    }

//    std::cout << "Request successful: " << resp.status() << std::endl;
//    std::cout << "Request successful: " << resp.status_message() << std::endl;
//    std::cout << "------ Content ------" << std::endl;
//    std::cout << resp.content() << std::endl;
//  });

  Request req;
  req.set_host("localhost");
  req.set_port(8000);

  req.set_method(HttpMethod::GET);
  req.set_path("/api/server.js");
  req.add_header("Host", "localhost:8000");
  req.add_header("Connection", "close");

//  na.Run(std::move(req), [](const std::error_code &error, Response resp) {
//    if (error) {
//      std::cout << "Request failed: " << error.message() << std::endl;
//      return;
//    }
//
//    std::cout << "Request successful: " << resp.status() << std::endl;
//    std::cout << "Request successful: " << resp.status_message() << std::endl;
//    std::cout << "------ Content ------" << std::endl;
//    std::cout << resp.content() << std::endl;
//  });


  na.Wait();

  std::cout << "NetworkAccess finished" << std::endl;
}
