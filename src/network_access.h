#ifndef NETWORK_ACCES_H
#define NETWORK_ACCES_H

#include "asio/io_service.hpp"
#include "asio/ssl/context.hpp"

#include "basic_request.h"

namespace restpp {
class NetworkAccess {
public:
  NetworkAccess() : ssl_context_(asio::ssl::context::sslv23_client) {}

  Request CreateRequest() {
    return Request(io_service_);
  }

  SslRequest CreateSslRequest() {
    return SslRequest(io_service_, ssl_context_);
  }

  inline void Run() { io_service_.run(); }
  inline void Stop() { io_service_.stop(); }
private:
  asio::io_service io_service_;
  asio::ssl::context ssl_context_;
};
}

#endif // NETWORK_ACCES_H
