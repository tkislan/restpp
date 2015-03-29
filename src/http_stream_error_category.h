#ifndef HTTP_STREAM_ERROR_CATEGORY
#define HTTP_STREAM_ERROR_CATEGORY

#include <system_error>

namespace restpp {
class HttpStreamErrorCategory : public std::error_category {
public:
  HttpStreamErrorCategory() {}

  enum HttpStreamError {
    INVALID_STATUS_LINE = 1,
    INVALID_HEADER_LINE = 2,
    INVALID_CHUNK_SIZE = 3,
    MISSING_CHUNK_CRLF = 4
  };

  const char *name() const noexcept override { return "HttpStreamErrorCategory"; }

  std::string message(int error) const override {
    switch (error) {
      case INVALID_STATUS_LINE:
        return "Invalid status line";
      case INVALID_HEADER_LINE:
        return "Invalid header line";
      case INVALID_CHUNK_SIZE:
        return "Invalid chunk size";
      case MISSING_CHUNK_CRLF:
        return "Missing chunk crlf after the data";
      default:
        return "Undefined error message";
    }
  }

  static const HttpStreamErrorCategory &category() {
    static const HttpStreamErrorCategory category;
    return category;
  }
};
}

#endif // HTTP_STREAM_ERROR_CATEGORY

