#ifndef MOCK_REQUEST_BUILDER
#define MOCK_REQUEST_BUILDER

#include "gmock/gmock.h"

#include "basic_request.h"

namespace restpp {
class MockRequestBuilder {
public:
  MOCK_METHOD1(set_method, bool(const std::string&));
  MOCK_METHOD2(add_header, bool(const std::string&, const std::string&));
  MOCK_CONST_METHOD0(host, const std::string&());
  MOCK_CONST_METHOD0(port, std::uint16_t());
};
}

#endif // MOCK_REQUEST_BUILDER
