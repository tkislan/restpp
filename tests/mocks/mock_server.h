#ifndef MOCK_SERVER
#define MOCK_SERVER

#include <thread>
#include <memory>

#include "asio/io_service.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"
#include "asio/streambuf.hpp"

namespace restpp {
class MockServer {
public:
  MockServer(std::uint16_t port)
    : work_(new asio::io_service::work(io_service_)),
      acceptor_(io_service_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      socket_(io_service_),
      thread_(static_cast<size_t (asio::io_service::*)()>(&asio::io_service::run), &io_service_)
  {}

  ~MockServer() {
    thread_.join();
  }

  void set_response(const std::string &response) { response_ = response; }

  void Listen() {
    acceptor_.async_accept(socket_, std::bind(&MockServer::AcceptCallback, this, std::placeholders::_1));
  }

  void AcceptCallback(const std::error_code &error) {
    if (error) {
      std::cout << "AcceptCallback: " << error.message() << std::endl;
      return Stop();
    }

    std::cout << "Accepted connection" << std::endl;

    AsyncRead();
    asio::async_write(socket_, asio::buffer(response_), std::bind(&MockServer::WriteCallback, this, std::placeholders::_1, std::placeholders::_2));
  }

  void AsyncRead() {
    asio::async_read(socket_, buffer_, std::bind(&MockServer::ReadCallback, this, std::placeholders::_1, std::placeholders::_2));
  }

  void ReadCallback(const std::error_code &error, size_t bytes_transferred) {
    if (error) {
      std::cout << "ReadCallback: " << error.message() << std::endl;
      return Stop();
    }

    std::cout << "ReadCallback read " << bytes_transferred << " bytes" << std::endl;

    AsyncRead();
  }

  void WriteCallback(const std::error_code &error, size_t bytes_transferred) {
    if (error) {
      std::cout << "WriteCallback: " << error.message() << std::endl;
      return Stop();
    }

    std::cout << "WriteCallback wrote " << bytes_transferred << " bytes" << std::endl;
  }

  void Stop() {
    work_.reset();
    io_service_.stop();
  }

private:
  asio::io_service io_service_;
  std::unique_ptr<asio::io_service::work> work_;
  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::socket socket_;
  asio::streambuf buffer_;

  std::string response_;

  std::thread thread_;
};
}

#endif // MOCK_SERVER

