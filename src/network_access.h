#ifndef NETWORK_ACCES_H
#define NETWORK_ACCES_H

#include "asio/io_service.hpp"
#include "asio/ssl/context.hpp"

#include "basic_request.h"

namespace restpp {
class NetworkAccess {
public:
  NetworkAccess();

  Request &&CreateRequest();
  SslRequest &&CreateSslRequest();

  inline void Run() { io_service_.run(); }
  inline void Stop() { io_service_.stop(); }
private:
  asio::io_service io_service_;
  asio::ssl::context ssl_context_;
};
}

#endif // NETWORK_ACCES_H