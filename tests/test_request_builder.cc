#include "gtest/gtest.h"

#include "basic_request.h"
#include "mocks/mock_request_builder.h"

using namespace restpp;

TEST(RequestBuilder, Simple) {
  BasicRequest<MockRequestBuilder> request;

  EXPECT_CALL(request.request_builder(), set_method("GET")).WillOnce(testing::Return(true));

  request.set_method(HttpMethod::GET);
}
