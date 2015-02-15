#include "network_access.h"

namespace restpp {
NetworkAccess::NetworkAccess()
  : ssl_context_(asio::ssl::context::sslv23_client)
{

}

Request &&NetworkAccess::CreateRequest() {
  Request req(io_service_);

  return std::move(req);
}

SslRequest &&NetworkAccess::CreateSslRequest() {
  SslRequest req(io_service_, ssl_context_);

  return std::move(req);
}

//bool NetworkAccess::Run(restpp::Request &request) {
//
//
//  return true;
//}
}