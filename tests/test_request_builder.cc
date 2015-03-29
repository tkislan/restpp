#include "gtest/gtest.h"

#include "request.h"
#include "request_builder.h"

using namespace restpp;

TEST(RequestBuilder, Simple) {
  Request request;
  RequestBuilder request_builder;

  request.set_method(HttpMethod::GET);
}
