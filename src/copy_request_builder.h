#ifndef COPY_REQUEST_BUILDER_H
#define COPY_REQUEST_BUILDER_H

#include <memory>
#include <ostream>

#include "asio/streambuf.hpp"
#include "asio/buffers_iterator.hpp"

namespace restpp {
//static const std::string kCrLf("\r\n");
static const std::string kCrLf("\n");

class CopyRequestBuilder {
public:
  inline bool set_method(const std::string &method) { method_ = method; return true; }
  inline bool set_path(const std::string &path) { path_ = path; return true; }
  inline bool add_query_param(const std::string &name, const std::string &value) { query_params_.push_back(std::make_pair(name, value)); return true; }

  inline bool add_header(const std::string &name, const std::string &value) { headers_.push_back(std::make_pair(name, value)); return true; }

  inline const asio::streambuf &buffer() const { return buffer_; }
  inline std::string buffer_string() const {
    asio::streambuf::const_buffers_type buffer = buffer_.data();
    return std::string(asio::buffers_begin(buffer), asio::buffers_begin(buffer) + buffer_.size());
  }

  inline bool Build() {
    std::ostream os(&buffer_);

    if (method_.empty()) return false;
    os << method_;

    if (path_.empty()) return false;
    os << ' ' << path_;

    bool first_query_param = true;
    for (const auto &query_param_pair : query_params_) {
      if (first_query_param) {
        os << '?';
        first_query_param = false;
      } else {
        os << '&';
      }
      os << query_param_pair.first << '=' << query_param_pair.second;
    }

    os << " HTTP/1.1" << kCrLf;

    for (const auto &header_pair : headers_) {
      os << header_pair.first << ": " << header_pair.second << kCrLf;
    }

    os << kCrLf;

    return true;
  }
private:
  asio::streambuf buffer_;

  std::string method_;
  std::string path_;
  std::vector<std::pair<std::string, std::string>> query_params_;
  std::vector<std::pair<std::string, std::string>> headers_;
};
}

#endif // COPY_REQUEST_BUILDER_H
