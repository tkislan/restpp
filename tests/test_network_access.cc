#include "gtest/gtest.h"

#include "network_access.h"

using namespace restpp;

TEST(TestNetworkAccess, Simple) {
  NetworkAccess na;

  Request req = na.CreateRequest();
}
