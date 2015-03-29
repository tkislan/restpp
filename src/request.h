#ifndef REQUEST_H
#define REQUEST_H

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <map>

namespace restpp {
enum class HttpMethod {
  GET = 0,
  POST = 1,
  PUT = 2,
  DELETE = 3
};

enum class ContentType {
  NONE = 0,
  COMPLETE = 1,
  CHUNKED = 2
};

static const std::vector<std::string> kHttpMethodString {
  "GET", "POST", "PUT", "DELETE"
};

static const std::uint16_t kDefaultHttpPort = 80;
static const std::uint16_t kDefaultHttpsPort = 443;

class Request {
public:
  //  typedef Socket SocketType;

  //  template<typename U = Socket>
  //  Request(typename std::enable_if<std::is_same<U, asio::ip::tcp::socket>::value, asio::io_service>::type &io_service)
  //    : strand_(std::make_shared<asio::io_service::strand>(io_service)),
  //      resolver_(new asio::ip::tcp::resolver(io_service)),
  //      socket_(new Socket(io_service)),
  //      request_builder_(new RequestBuilder),
  //      response_reader_(new ResponseReader<Socket>(socket_, strand_)),
  //      port_(kDefaultHttpPort)
  //  {}

  //  template<typename U = Socket>
  //  Request(typename std::enable_if<std::is_same<U, asio::ssl::stream<asio::ip::tcp::socket>>::value, asio::io_service>::type &io_service, asio::ssl::context &ssl_context)
  //    : strand_(std::make_shared<asio::io_service::strand>(io_service)),
  //      resolver_(new asio::ip::tcp::resolver(io_service)),
  //      socket_(new Socket(io_service, ssl_context)),
  //      request_builder_(new RequestBuilder),
  //      response_reader_(new ResponseReader<Socket>(socket_, strand_)),
  //      port_(kDefaultHttpsPort)
  //  {}

  Request() {}

  Request(Request &&request) = default;

  //  Request() = delete;
  Request(const Request &request) = delete;
  Request &operator=(Request &request) = delete;

  void Run(const std::function<void (const std::error_code&)> &callback);

  inline const std::string &host() const { return host_; }
  inline void set_host(const std::string &host) { host_ = host; }
  inline std::uint16_t port() const { return port_; }
  inline void set_port(std::uint16_t port) { port_ = port; }

  inline const std::string &method() const { return method_; }
  inline void set_method(const std::string &method) { method_ = method; }
  inline void set_method(HttpMethod method) { method_ = kHttpMethodString[static_cast<size_t>(method)]; }

  inline const std::string &path() const { return path_; }
  inline void set_path(const std::string &path) { path_ = path; }

  inline const std::vector<std::pair<std::string, std::string>> &query_params() const { return query_params_; }

  inline void add_query_param(const std::string &name, const std::string &value) {
    query_params_.push_back(std::make_pair(name, value));
  }

  template<typename Value>
  inline typename std::enable_if<std::is_integral<Value>::value, void>::type
   add_query_param(const std::string &name, const Value &value) {
    query_params_.push_back(std::make_pair(name, std::to_string(value)));
  }

  inline const std::map<std::string, std::string> &headers() const { return headers_; }

  inline void add_header(const std::string &name, const std::string &value) {
    headers_.insert(std::make_pair(name, value));
  }

  template<typename Value>
  inline typename std::enable_if<std::is_integral<Value>::value, void>::type
  add_header(const std::string &name, const Value &value) {
    headers_.insert(std::make_pair(name, std::to_string(value)));
  }

  inline ContentType content_type() const { return content_type_; }
  inline bool has_content() const { return content_type_ != ContentType::NONE; }

  inline const std::string &content() const { return content_; }
  inline void set_content(const std::string &content) {
    content_ = content;
    content_type_ = ContentType::COMPLETE;
  }

private:
  std::string host_;
  std::uint16_t port_ = 0;

  std::string method_;

  std::string path_;

  std::vector<std::pair<std::string, std::string>> query_params_;
  std::map<std::string, std::string> headers_;

  ContentType content_type_ = ContentType::NONE;
  std::string content_;
};

//template<typename Socket, typename RequestBuilder>
//void Request<Socket, RequestBuilder>::Run(const std::function<void (const std::error_code&)> &callback) {
//  bool ret = request_builder_->Build();
//  assert(ret);

////  std::cout << &request_builder_->buffer() << std::endl;

//  asio::ip::tcp::resolver::query query(host_, std::to_string(port_), asio::ip::resolver_query_base::numeric_service);

//  resolver_->async_resolve(query, std::bind(&Request<Socket, RequestBuilder>::ResolveCallback, this, std::placeholders::_1, std::placeholders::_2, callback));
//}
}

#endif // REQUEST_H
