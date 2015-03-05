#ifndef BASIC_REQUEST_H
#define BASIC_REQUEST_H

#include <memory>
#include <functional>
#include <string>

#include "asio/io_service.hpp"
#include "asio/io_service_strand.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ssl/context.hpp"
#include "asio/ssl/stream.hpp"
#include "asio/connect.hpp"

#include "stream_request_builder.h"
#include "response_reader.h"

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

static const std::uint16_t kDefaultHttpPort = 80;
static const std::uint16_t kDefaultHttpsPort = 443;

template<typename Socket, typename RequestBuilder>
class BasicRequest {
public:
  template<typename U = Socket>
  BasicRequest(typename std::enable_if<std::is_same<U, asio::ip::tcp::socket>::value, asio::io_service>::type &io_service)
    : strand_(std::make_shared<asio::io_service::strand>(io_service)),
      resolver_(new asio::ip::tcp::resolver(io_service)),
      socket_(new Socket(io_service)),
      request_builder_(new RequestBuilder),
      response_reader_(new ResponseReader<Socket>(socket_, strand_)),
      port_(kDefaultHttpPort)
  {}

  template<typename U = Socket>
  BasicRequest(typename std::enable_if<std::is_same<U, asio::ssl::stream<asio::ip::tcp::socket>>::value, asio::io_service>::type &io_service, asio::ssl::context &ssl_context)
    : strand_(std::make_shared<asio::io_service::strand>(io_service)),
      resolver_(new asio::ip::tcp::resolver(io_service)),
      socket_(new Socket(io_service, ssl_context)),
      request_builder_(new RequestBuilder),
      response_reader_(new ResponseReader<Socket>(socket_, strand_)),
      port_(kDefaultHttpsPort)
  {}

  BasicRequest(BasicRequest &&request) = default;

  BasicRequest() = delete;
  BasicRequest(const BasicRequest &request) = delete;

  BasicRequest &operator=(BasicRequest &request) = default;

  void Run(const std::function<void (const std::error_code&)> &callback);

  inline void set_host(const std::string &host) { host_ = host; }
  inline void set_port(std::uint16_t port) { port_ = port; }

  inline bool set_method(const std::string &method) { return request_builder_->set_method(method); }
  inline bool set_method(HttpMethod method) { return request_builder_->set_method(kHttpMethodString[static_cast<size_t>(method)]); }
  inline bool set_path(const std::string &path) { return request_builder_->set_path(path); }

  template<typename Value>
  inline bool add_query_param(const std::string &name, const Value &value) { return request_builder_->add_query_param(name, value); }

  template<typename Value>
  inline bool add_header(const std::string &name, const Value &value) { return request_builder_->add_header(name, value); }

  inline asio::streambuf &buffer() { return request_builder_->buffer(); }

  inline RequestBuilder &request_builder() { return *request_builder_; }

  void ResolveCallback(const std::error_code &error,
                       asio::ip::tcp::resolver::iterator endpoint_iterator,
                       const std::function<void (const std::error_code&)> &callback)
  {
    if (error) return callback(error);

    asio::async_connect(*socket_, endpoint_iterator, std::bind(&BasicRequest<Socket, RequestBuilder>::ConnectCallback, this, std::placeholders::_1, callback));
  }

  void ConnectCallback(const std::error_code &error, const std::function<void (const std::error_code&)> &callback) {
    if (error) return callback(error);

    asio::async_write(*socket_, buffer(), strand_->wrap(std::bind(&BasicRequest<Socket, RequestBuilder>::WriteCallback, this, std::placeholders::_1, callback)));
    response_reader_->Init([](const std::error_code &error) {
      std::cout << "Response reader finished with error: " << error << ' ' << error.message() << std::endl;
    });
  }

  void WriteCallback(const std::error_code &error, const std::function<void (const std::error_code&)> &callback) {
    if (error) return callback(error);

//    callback(std::error_code());
  }

private:
  std::shared_ptr<asio::io_service::strand> strand_;

  std::unique_ptr<asio::ip::tcp::resolver> resolver_;

  std::shared_ptr<Socket> socket_;
  std::unique_ptr<RequestBuilder> request_builder_;
  std::unique_ptr<ResponseReader<Socket>> response_reader_;

  std::string host_;
  std::uint16_t port_;
};

typedef BasicRequest<asio::ip::tcp::socket, StreamRequestBuilder> Request;
typedef BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>, StreamRequestBuilder> SslRequest;

template<typename Socket, typename RequestBuilder>
void BasicRequest<Socket, RequestBuilder>::Run(const std::function<void (const std::error_code&)> &callback) {
  bool ret = request_builder_->Build();
  assert(ret);

  asio::ip::tcp::resolver::query query(host_, std::to_string(port_), asio::ip::resolver_query_base::numeric_service);

  resolver_->async_resolve(query, std::bind(&BasicRequest<Socket, RequestBuilder>::ResolveCallback, this, std::placeholders::_1, std::placeholders::_2, callback));
}
}

#endif // BASIC_REQUEST_H
