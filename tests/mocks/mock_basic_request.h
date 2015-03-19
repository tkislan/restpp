#ifndef MOCK_BASIC_REQUEST
#define MOCK_BASIC_REQUEST

#include "gmock/gmock.h"

namespace restpp {
class MockBasicRequest {
public:
  MockBasicRequest() = default;
  MockBasicRequest(MockBasicRequest &&other) = default;

  MOCK_CONST_METHOD0(host, const std::string&());
  MOCK_CONST_METHOD0(port, std::uint16_t());
  MOCK_METHOD0(Build, bool());
  MOCK_METHOD0(buffer, asio::streambuf&());
};
}

#endif // MOCK_BASIC_REQUEST

