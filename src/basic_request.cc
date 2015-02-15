#include "basic_request.h"

namespace restpp {
template<typename Socket>
void BasicRequest<Socket>::Run(const std::function<void()> &callback) {
  asio::async_write(asio::buffer(buffer_), [callback](const std::error_code &error, size_t bytes_transferred) {
    callback();
  });

//  callback();
}
}