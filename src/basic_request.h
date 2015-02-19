#ifndef BASIC_REQUEST_H
#define BASIC_REQUEST_H

#include <memory>
#include <functional>
#include <string>

#include "asio/io_service.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ssl/context.hpp"
#include "asio/ssl/stream.hpp"
#include "asio/placeholders.hpp"

#include "stream_request_builder.h"

namespace restpp {
enum class HttpMethod {
  GET = 0,
  POST = 1,
  PUT = 2,
  DELETE = 3
};

static const std::vector<std::string> kHttpMethodString {
  "GET", "POST", "PUT", "DELETE"
};

template<typename Socket, typename RequestBuilder>
class BasicRequest {
public:
  template<typename U = Socket>
  BasicRequest(typename std::enable_if<std::is_same<U, asio::ip::tcp::socket>::value, asio::io_service>::type &io_service)
    : socket_(new Socket(io_service)),
      request_builder_(new RequestBuilder)
  {}

  template<typename U = Socket>
  BasicRequest(typename std::enable_if<std::is_same<U, asio::ssl::stream<asio::ip::tcp::socket>>::value, asio::io_service>::type &io_service, asio::ssl::context &ssl_context)
    : socket_(new Socket(io_service, ssl_context)),
      request_builder_(new RequestBuilder)
  {}

  BasicRequest(BasicRequest &&request) = default;

  BasicRequest() = delete;
  BasicRequest(const BasicRequest &request) = delete;

  BasicRequest &operator=(BasicRequest &request) = default;

  void Run(const std::function<void (const std::error_code&)> &callback);

  inline void set_host(const std::string &host) { host_ = host; }


  inline bool set_method(const std::string &method) { return request_builder_->set_method(method); }
  inline bool set_method(HttpMethod method) { return request_builder_->set_method(kHttpMethodString[static_cast<size_t>(method)]); }
  inline bool set_path(const std::string &path) { return request_builder_->set_path(path); }
  inline bool add_query_param(const std::string &name, const std::string &value) { return request_builder_->add_query_param(name, value); }
  inline bool add_header(const std::string &name, const std::string &value) { return request_builder_->add_header(name, value); }
  inline asio::streambuf &buffer() { return request_builder_->buffer(); }

  void WriteCallback(const std::error_code &error, const std::function<void (const std::error_code&)> &callback) {
    if (error) return callback(error);
  }

private:
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<RequestBuilder> request_builder_;

  std::string host_;
};

typedef BasicRequest<asio::ip::tcp::socket, StreamRequestBuilder> Request;
typedef BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>, StreamRequestBuilder> SslRequest;

template<typename Socket, typename RequestBuilder>
void BasicRequest<Socket, RequestBuilder>::Run(const std::function<void (const std::error_code&)> &callback) {
  asio::async_write(*socket_, buffer(), std::bind(&BasicRequest<Socket, RequestBuilder>::WriteCallback, this, std::placeholders::_1, callback));
}
}

#endif // BASIC_REQUEST_H
