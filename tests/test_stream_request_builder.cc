#include "gtest/gtest.h"

#include "stream_request_builder.h"

using namespace restpp;

class TestStreamRequestBuilder : public testing::Test {
public:
  StreamRequestBuilder builder_;
};

TEST_F(TestStreamRequestBuilder, MissingMethod) {
  ASSERT_EQ(builder_.Build(), false);

  ASSERT_EQ(builder_.set_method("GET"), true);
  ASSERT_EQ(builder_.Build(), false);

  ASSERT_EQ(builder_.set_path("/"), true);
  ASSERT_EQ(builder_.Build(), true);
}

TEST_F(TestStreamRequestBuilder, test) {
//  ASSERT_EQ(builder_.set_method("PUT"), true);
//  ASSERT_EQ(builder_.set_path("/api/mold/history/cycle-comments/add"), true);
//  ASSERT_EQ(builder_.add_header("Content-type", "application/json"), true);
//  ASSERT_EQ(builder_.add_header("Host", "localhost"), true);
//  ASSERT_EQ(builder_.add_header("Accept", "*/*"), true);

//  ASSERT_EQ(builder_.set_content("{\"1\":{\"i32\":269},\"2\":{\"str\":\"safaadgsadgsdgsadgsd\"}}"), true);

  ASSERT_EQ(builder_.set_method("GET"), true);
  ASSERT_EQ(builder_.set_path("/api/mold/history/cycle-comments"), true);
  ASSERT_EQ(builder_.add_header("Host", "localhost"), true);

  ASSERT_EQ(builder_.Build(), true);

  std::cout << builder_.buffer_string() << std::endl;
}
