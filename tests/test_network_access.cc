#include "gtest/gtest.h"

#include "network_access.h"

using namespace restpp;

TEST(TestNetworkAccess, Simple) {
  NetworkAccess na;

  Request req = na.CreateRequest();

  req.set_method(HttpMethod::GET);

  req.Run([](const std::error_code &error) {

  });
}
