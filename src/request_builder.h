#ifndef REQUEST_BUILDER_H
#define REQUEST_BUILDER_H

#include <system_error>
#include <ostream>

#include "asio/streambuf.hpp"

#include "request.h"
#include "utils.h"

namespace restpp {
class RequestBuilder {
public:
  static void BuildRequest(const Request &req, asio::streambuf &buffer, std::error_code &error) {
    std::ostream os(&buffer);

    if (req.method().empty()) return;
    os << req.method();

    if (req.path().empty()) return;
    os << ' ' << req.path();

    bool first_query_param = true;
    for (const auto &query_param_pair : req.query_params()) {
      if (first_query_param) {
        os << '?';
        first_query_param = false;
      } else {
        os << '&';
      }
      os << query_param_pair.first << '=' << query_param_pair.second;
    }

    os << " HTTP/1.1" << kCrLf;

    for (const auto &header_pair : req.headers()) {
      os << header_pair.first << ": " << header_pair.second << kCrLf;
    }

    if (req.content_type() == ContentType::COMPLETE) {
      os << "Content-length" << ": " << std::to_string(req.content().size()) << kCrLf;
    }
    
    os << kCrLf;
  }
};
}

#endif // REQUEST_BUILDER_H
