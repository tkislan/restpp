#ifndef HTTP_CONNECTOR_H
#define HTTP_CONNECTOR_H

#include "asio/io_service.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ssl/stream.hpp"
#include "asio/ssl/context.hpp"

namespace restpp {
template<typename SocketType>
class HttpConnector : public std::enable_shared_from_this<HttpConnector<SocketType>> {
public:
  template<typename U = SocketType>
  HttpConnector(typename std::enable_if<std::is_same<U, asio::ip::tcp::socket>::value, asio::io_service>::type &io_service)
    : resolver_(io_service),
      socket_(io_service)
  {}

  template<typename U = SocketType>
  HttpConnector(typename std::enable_if<std::is_same<U, asio::ssl::stream<asio::ip::tcp::socket>>::value, asio::io_service>::type &io_service, asio::ssl::context &ssl_context)
    : resolver_(io_service),
      socket_(io_service, ssl_context)
  {}

  void Connect(const std::function<void (const std::error_code&, SocketType &&socket)> &callback) {
    callback(std::error_code(), std::move(socket_));
  }

private:
  asio::ip::tcp::resolver resolver_;
  SocketType socket_;
};
}

#endif // HTTP_CONNECTOR_H

