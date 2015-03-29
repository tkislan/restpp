#ifndef NETWORK_ACCES_H
#define NETWORK_ACCES_H

#include <thread>
#include <mutex>

#include "asio/io_service.hpp"
#include "asio/ssl/context.hpp"
#include "asio/ip/tcp.hpp"

//#include "basic_request.h"
#include "request.h"
#include "copy_request_builder.h"
#include "http_stream.h"

namespace restpp {
static const size_t kMaxThreadCount = 4;

static size_t SafeThreadCount(size_t thread_count) { return std::min(std::max<size_t>(1u, thread_count), kMaxThreadCount); }

class NetworkAccess {
public:
  NetworkAccess(size_t thread_count = 1)
    : io_service_(SafeThreadCount(thread_count)),
      work_(new asio::io_service::work(io_service_)),
      ssl_context_(asio::ssl::context::sslv23_client),
      running_(false)
  {
    thread_count = SafeThreadCount(thread_count);
    for (size_t i = 0; i < thread_count; ++i) {
      thread_group_.push_back(std::thread(static_cast<size_t (asio::io_service::*)()>(&asio::io_service::run), &io_service_));
    }
  }


  ~NetworkAccess() {
    work_.reset();
    for (auto &t : thread_group_) t.join();
  }

//  template<typename RequestBuilder = CopyRequestBuilder>
//  BasicRequest<asio::ip::tcp::socket, RequestBuilder> CreateRequest() {
//    return BasicRequest<asio::ip::tcp::socket, RequestBuilder>(io_service_);
//  }

//  template<typename RequestBuilder = CopyRequestBuilder>
//  BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>, RequestBuilder> CreateSslRequest() {
//    return BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>, RequestBuilder>(io_service_, ssl_context_);
//  }

//  template<typename RequestBuilder = CopyRequestBuilder>
//  BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>, RequestBuilder> CreateSslRequest(asio::ssl::context &ssl_context) {
//    return BasicRequest<asio::ssl::stream<asio::ip::tcp::socket>, RequestBuilder>(io_service_, ssl_context);
//  }

//  template<typename Request>
//  inline std::shared_ptr<HttpStream<asio::ip::tcp::socket>> Run(Request &&request,
//                                                                std::function<void (const std::error_code&, Response response)> callback)
//  {
//    auto http = std::make_shared<HttpStream<asio::ip::tcp::socket>>(io_service_, std::move(request), std::move(callback));
//    http->Run();
//    return http;
//  }

  inline void Wait() {
    work_.reset();

    for (auto &t : thread_group_) t.join();
    thread_group_.clear();
  }

  inline void Stop() {
    io_service_.stop();
    Wait();
  }

private:
  asio::io_service io_service_;
  std::unique_ptr<asio::io_service::work> work_;
  asio::ssl::context ssl_context_;

  std::mutex mutex_;
  bool running_;
  std::vector<std::thread> thread_group_;
};
}

#endif // NETWORK_ACCES_H
