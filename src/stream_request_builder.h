#ifndef STREAM_REQUEST_BUILDER_H
#define STREAM_REQUEST_BUILDER_H

#include <cassert>

#include <memory>
#include <ostream>

#include "asio/streambuf.hpp"
#include "asio/buffers_iterator.hpp"

#include "constants.h"

namespace restpp {
class StreamRequestBuilder {
public:
  StreamRequestBuilder()
    : stream_(&buffer_)
  {}

  StreamRequestBuilder(StreamRequestBuilder &&builder) = default;

  inline bool set_method(const std::string &method) {
    if (state_ != State::kMethod) { assert(state_ == State::kMethod); return false; }

    stream_ << method;

    state_ = State::kPath;

    return true;
  }

  inline bool set_path(const std::string &path) {
    if (state_ != State::kPath) { assert(state_ == State::kPath); return false; }

    stream_ << " " << path;

    state_ = State::kQueryParam;

    return true;
  }


  template<typename Value>
  inline bool add_query_param(const std::string &name, const Value &value) {
    if (state_ != State::kQueryParam) { assert(state_ == State::kQueryParam); return false; }

    if (first_query_param_) {
      stream_ << '?';
      first_query_param_ = false;
    } else {
      stream_ << '&';
    }

    stream_ << name << '=' << value;

    return true;
  }

  template<typename Value>
  inline bool add_header(const std::string &name, const Value &value) {
    if (state_ == State::kQueryParam) {
      close_path_line();
      state_ = State::kHeader;
    } else if (state_ != State::kHeader) {
      assert(state_ == State::kHeader);
      return false;
    }

    stream_ << name << ": " << value << kCrLf;

    return true;
  }

  inline bool set_content(const std::string &content) {
    if (!add_header("Content-Length", content.size())) {
      assert(!"Failed to add Content-Length header");
      return false;
    }

    if (state_ == State::kQueryParam) {
      close_path_line();
      state_ = State::kContent;
    } else if (state_ == State::kHeader) {
      stream_ << kCrLf;
    } else if (state_ == State::kContent) {
      assert(state_ != State::kContent);
      return false;
    }

    state_ = State::kContent;

    stream_ << content;

    return true;
  }

  inline bool Build() {
    if (state_ == State::kMethod || state_ == State::kPath) return false;
    if (state_ == State::kQueryParam) close_path_line();
    if (state_ == State::kHeader || state_ == State::kContent) stream_ << kCrLf;

    return true;
  }

  inline asio::streambuf &buffer() { return buffer_; }
  inline std::string buffer_string() const {
    asio::streambuf::const_buffers_type buffer = buffer_.data();
    return std::string(asio::buffers_begin(buffer), asio::buffers_begin(buffer) + buffer_.size());
  }

private:
  enum class State {
    kMethod,
    kPath,
    kQueryParam,
    kHeader,
    kContent
  };

  inline void close_path_line() { stream_ << " HTTP/1.1" << kCrLf; }

  State state_ = State::kMethod;
  bool first_query_param_ = true;

  asio::streambuf buffer_;
  std::ostream stream_;
};
}
#endif // STREAM_REQUEST_BUILDER_H
