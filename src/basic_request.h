#ifndef BASIC_REQUEST_H
#define BASIC_REQUEST_H

#include <memory>
#include <functional>
#include <string>
#include <sstream>

#include "asio/io_service.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ssl/context.hpp"
#include "asio/ssl/stream.hpp"

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

static const std::string kCrLf("\r\n");

template<typename SocketType>
class BasicRequest {
public:
  BasicRequest<asio::ip::tcp::socket>(asio::io_service &io_service) : socket_(io_service) {}
  BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>>(asio::io_service &io_service, asio::ssl::context &ssl_context) : socket_(io_service, ssl_context) {}
  BasicRequest(BasicRequest &&request) = default;

  BasicRequest() = delete;
  BasicRequest(const BasicRequest &request) = delete;

  void Run(const std::function<void()> &callback);

  inline void set_host(const std::string &host) {
    if (state_ != State::kHost) { assert(state_ == State::kHost); return; }

    host_ = host;

    ++state_;
  }

  inline void set_method(const std::string &method) {
    if (state_ != State::kMethod) { assert(state_ == State::kMethod); return; }

    buffer_ << method;

    ++state_;
  }

  inline void set_method(HttpMethod method) {
    if (state_ != State::kMethod) { assert(state_ == State::kMethod); return; }

    buffer_ << kHttpMethodString[static_cast<size_t>(method)];

    ++state_;
  }

  inline void set_path(const std::string &path) {
    if (state_ != State::kPath) { assert(state_ == State::kPath); return; }

    buffer_ << " " << path;
  }


  inline void add_query_param(const std::string &name, const std::string &value) {
    if (state_ == State::kPath) {
      buffer_ << '?';
      ++state_;
    } else if (state_ == State::kQueryParam) {
      buffer_ << '&';
    } else {
      assert(state_ == State::kQueryParam); return;
    }

    buffer_ << name << '=' << value;
  }

  inline void add_header(const std::string &name, const std::string &value) {
    if (state_ == State::kQueryParam) {
      close_path_line();
      ++state_;
    } else if (state_ != State::kHeader) {
      assert(state_ == State::kHeader);
      return;
    }

    buffer_ << name << ": " << value << kCrLf;
  }

private:
  enum class State {
    kHost,
    kMethod,
    kPath,
    kQueryParam,
    kHeader
  };

  inline void close_path_line() { buffer_ << "HTTP/1.1" << kCrLf; }

  State state_ = State::kHost;

  SocketType socket_;

  bool first_query_param_ = true;

  std::string host_;
  std::stringstream buffer_;
};

typedef BasicRequest<asio::ip::tcp::socket> Request;
typedef BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>> SslRequest;
}

#endif // BASIC_REQUEST_H
