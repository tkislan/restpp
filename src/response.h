#ifndef RESPONSE
#define RESPONSE

#include <string>
#include <vector>
#include <map>

#include "utils.h"

namespace restpp {
class Response {
  template<typename RequestType, typename SocketType> friend class HttpStream;
public:
  Response() = default;
  Response(Response &&response) = default;
  Response &operator=(Response &&response) = default;

//  Response(Response &&other) {
//    status_ = other.status_; other.status_ = 0;
//    method_ = std::move(other.method_);
//    query_params_ = std::move(other.query_params_);
//    headers_ = std::move(other.headers_);
//  }

  Response(const Response &response) = delete;
  Response &operator=(const Response &response) = delete;

  inline unsigned int status() const { return status_; }
  inline const std::string &status_message() const { return status_message_; }

  const std::map<std::string, std::string, ci_less> &headers() const { return headers_; }

  const std::string &content() const { return content_; }

private:
  std::string http_version_;
  unsigned int status_ = 0;
  std::string status_message_;

//  std::vector<std::pair<std::string, std::string>> headers_;
  std::map<std::string, std::string, ci_less> headers_;

  std::string content_;
};
}

#endif // RESPONSE

