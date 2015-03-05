#include "gtest/gtest.h"

#include "network_access.h"

using namespace restpp;

TEST(TestNetworkAccess, Simple) {
  NetworkAccess na;

  Request req = na.CreateRequest();

  req.set_host("localhost");
  req.set_port(8000);

  req.set_method(HttpMethod::GET);
  req.set_path("/api");
  req.add_header("Host", "localhost:8000");
  req.add_header("Connection", "close");

  req.Run([](const std::error_code &error) {
    std::cout << error.message() << std::endl;

    EXPECT_EQ(static_cast<bool>(error), false);
  });

  na.Run();
}
