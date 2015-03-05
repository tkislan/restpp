#ifndef RESPONSE_READER
#define RESPONSE_READER

#include <system_error>
#include <memory>
#include <functional>

#include "asio/io_service_strand.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/streambuf.hpp"

#include "response.h"

namespace restpp {
template<typename Socket>
class ResponseReader {
public:
  ResponseReader(const std::shared_ptr<Socket> &socket, const std::shared_ptr<asio::io_service::strand> &strand)
    : socket_(socket),
      strand_(strand)
  {}

  void Init(const std::function<void (const std::error_code&)> &callback) {
    asio::async_read_until(*socket_, buffer_, "\r\n", strand_->wrap(std::bind(&ResponseReader::ReadStatusLineCallback, this, std::placeholders::_1, callback)));
  }

private:
  void ReadStatusLineCallback(const std::error_code &error, const std::function<void (const std::error_code&)> &callback) {
    if (error) {
      return callback(error);
    }

    std::istream status_line(&buffer_);

    std::string http_version;
    status_line >> http_version;

    unsigned int status;
    status_line >> status;

    std::string status_message;
//    status_line >> status_message;
    std::getline(status_line, status_message);

    std::cout << http_version << ' ' << status << ' ' << status_message << std::endl;

    asio::async_read_until(*socket_, buffer_, "\r\n\r\n", strand_->wrap(std::bind(&ResponseReader::ReadHeadersCallback, this, std::placeholders::_1, callback)));
  }

  void ReadHeadersCallback(const std::error_code &error, const std::function<void (const std::error_code&)> &callback) {
    if (error) {
      return callback(error);
    }

//    callback(error);


    std::istream header_stream(&buffer_);
    std::string header_line;

    while (std::getline(header_stream, header_line) && header_line != "\r") {
      std::cout << header_line << std::endl;
    }

    std::cout << &buffer_;

    asio::async_read(*socket_, buffer_,
                     asio::transfer_at_least(1),
                     strand_->wrap(std::bind(&ResponseReader::ReadContentCallback, this, std::placeholders::_1, callback)));

//    asio::async_read_until(*socket_, buffer_, "\r\n", strand_->wrap(std::bind(&ResponseReader::ReadHeaderCallback, this, std::placeholders::_1, callback)));
  }

  void ReadContentCallback(const std::error_code &error, const std::function<void (const std::error_code&)> &callback) {
    if (error) {
      return callback(error);
    }

//    std::istream stream(&buffer_);
//    std::string str;
//    str << stream;

//    std::cout << str;

    std::cout << &buffer_;

    asio::async_read(*socket_, buffer_,
                     asio::transfer_at_least(1),
                     strand_->wrap(std::bind(&ResponseReader::ReadContentCallback, this, std::placeholders::_1, callback)));
  }

  Response response_;

  std::shared_ptr<Socket> socket_;
  std::shared_ptr<asio::io_service::strand> strand_;

  std::string content_;

  asio::streambuf buffer_;
};
}

#endif // RESPONSE_READER

