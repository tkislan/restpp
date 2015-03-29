#ifndef HTTP_STREAM
#define HTTP_STREAM

#include <memory>
#include <functional>

#include "asio/connect.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/write.hpp"
#include "asio/strand.hpp"
#include "asio/steady_timer.hpp"
#include "asio/ip/tcp.hpp"

#include "http_stream_error_category.h"
//#include "basic_request.h"
#include "response.h"
#include "request.h"
#include "utils.h"
#include "request_builder.h"

namespace restpp {
template<typename SocketType>
class HttpStream : public std::enable_shared_from_this<HttpStream<SocketType>> {
public:
  HttpStream(asio::io_service &io_service,
             Request &&request,
             std::function<void (const std::error_code&, Response response)> &&callback)
    : strand_(io_service),
      resolver_(io_service),
      socket_(io_service),
      timer_(io_service),
      request_(std::move(request)),
      data_callback_(std::bind(&HttpStream<SocketType>::MemoryDataCallback,
                               this,
                               std::placeholders::_1,
                               std::placeholders::_2,
                               std::placeholders::_3)),
      callback_(std::move(callback))
  {

  }

//  HttpStream(asio::io_service &io_service,
//             RequestType &&request,
//             std::function<void (const std::error_code&, Response response)> &&callback,
//             std::function<void (asio::buffers_iterator<asio::streambuf::const_buffers_type> begin,
//                                 asio::buffers_iterator<asio::streambuf::const_buffers_type> end,
//                                 std::error_code &error)> &&data_callback)
//    : strand_(io_service),
//      resolver_(io_service),
//      socket_(io_service),
//      timer_(io_service),
//      request_(std::move(request)),
//      data_callback_(std::move(data_callback)),
//      callback_(std::move(callback))
//  {

//  }



  ~HttpStream() {
    CloseSocket();
  }

  void Run() {
//    request_.Build();

    std::error_code error;
    RequestBuilder::BuildRequest(request_, request_buffer_, error);

    asio::streambuf::const_buffers_type bufs = request_buffer_.data();
    std::string request_string(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + request_buffer_.size());

    std::cout << request_string << std::endl;


    asio::ip::tcp::resolver::query query(request_.host(), std::to_string(request_.port()), asio::ip::resolver_query_base::numeric_service);

    resolver_.async_resolve(query, std::bind(&HttpStream<SocketType>::ResolveCallback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));

    StartTimer();
  }

private:
  void ResolveCallback(const std::error_code &error,
                       asio::ip::tcp::resolver::iterator endpoint_iterator)
  {
    if (error) return ErrorCallback(error);

    StartTimer();

    asio::async_connect(socket_, endpoint_iterator, std::bind(&HttpStream<SocketType>::ConnectCallback, this->shared_from_this(), std::placeholders::_1));
  }

  void ConnectCallback(const std::error_code &error) {
    if (error) return ErrorCallback(error);

    std::cout << "Connected" << std::endl;

    StartTimer();

    asio::async_write(socket_, request_buffer_, strand_.wrap(std::bind(&HttpStream<SocketType>::WriteRequestCallback,
                                                                       this->shared_from_this(),
                                                                       std::placeholders::_1,
                                                                       std::placeholders::_2)));

    asio::async_read_until(socket_, buffer_, kCrLf, strand_.wrap(std::bind(&HttpStream<SocketType>::ReadStatusLineCallback,
                                                                           this->shared_from_this(),
                                                                           std::placeholders::_1,
                                                                           std::placeholders::_2)));
  }

  void WriteRequestCallback(const std::error_code &error, size_t bytes_transferred) {
    if (error) return ErrorCallback(error);

    std::cout << "Written request: " << bytes_transferred << " bytes" << std::endl;

    switch (request_.content_type()) {
      case ContentType::NONE:
        break;
      case ContentType::COMPLETE:
        asio::async_write(socket_, asio::buffer(request_.content()), strand_.wrap(std::bind(&HttpStream<SocketType>::WriteRequestContentCallback,
                                                                                            this->shared_from_this(),
                                                                                            std::placeholders::_1,
                                                                                            std::placeholders::_2)));
        break;
      case ContentType::CHUNKED:
        break;
    }
  }

  void WriteRequestContentCallback(const std::error_code &error, size_t bytes_transferred) {
    std::cout << "WriteRequestContentCallback: " << error << ", bytes_transferred: " << bytes_transferred << std::endl;

    if (error) return ErrorCallback(error);
  }

  void ReadStatusLineCallback(const std::error_code &error, size_t bytes_transferred) {
    if (error) return ErrorCallback(error);

    std::cout << "ReadStatusLineCallback read: " << bytes_transferred << std::endl;

    std::cout << "Status line read" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << StreambufToString(buffer_) << std::endl;
    std::cout << "==========================================" << std::endl;

    std::istream is(&buffer_);

    GetStatusLine(is, response_.http_version_, response_.status_, response_.status_message_);
    if (!is.good()) return ErrorCallback(std::error_code(HttpStreamErrorCategory::INVALID_STATUS_LINE, HttpStreamErrorCategory::category()));

    asio::async_read_until(socket_, buffer_, kDoubleCrLf, strand_.wrap(std::bind(&HttpStream<SocketType>::ReadHeadersCallback,
                                                                                 this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2)));
  }

  void ReadHeadersCallback(const std::error_code &error, size_t bytes_transferred) {
    if (error) return ErrorCallback(error);

    std::cout << "ReadHeadersCallback read: " << bytes_transferred << std::endl;

    std::istream is(&buffer_);
    std::string header_name, header_value;

    do {
      GetHeader(is, header_name, header_value);
      if (!is.good()) return ErrorCallback(std::error_code(HttpStreamErrorCategory::INVALID_HEADER_LINE, HttpStreamErrorCategory::category()));

      if (!header_name.empty() && !header_value.empty()) {
        std::cout << "Header | " << header_name << ": " << header_value << std::endl;
        response_.headers_.insert(std::make_pair(header_name, header_value));
      }
    } while (!header_name.empty() && !header_value.empty());

    auto it = response_.headers().find("content-length");
    if (it != response_.headers().end()) {
      auto content_length = std::stoull(it->second);

      ReadContentFromBuffer();

      if (response_.content_.size() < content_length) AsyncReadContent(content_length);
      else FinishedCallback();

      return;
    }

    it = response_.headers().find("transfer-encoding");
    if (it != response_.headers().end() && it->second == "chunked") {
      AsyncReadChunkSize();

      return;
    }

    return FinishedCallback();
  }

  void AsyncReadContent(size_t content_length) {
    StartTimer();

    asio::async_read(socket_, buffer_, strand_.wrap(std::bind(&HttpStream<SocketType>::ReadContentCallback,
                                                              this->shared_from_this(),
                                                              std::placeholders::_1,
                                                              std::placeholders::_2,
                                                              content_length)));
  }

  void ReadContentFromBuffer() {
    if (buffer_.size() > 0) {
      std::error_code error;
      asio::streambuf::const_buffers_type bufs = buffer_.data();
      data_callback_(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + buffer_.size(), error);
      buffer_.consume(buffer_.size());
    }
  }

  void ReadContentCallback(const std::error_code &error, size_t bytes_transferred, size_t content_length) {
    std::cout << "Bytes read: " << bytes_transferred << std::endl;

    if (error && error.value() != asio::error::eof) return ErrorCallback(error);

    ReadContentFromBuffer();

    if (response_.content_.size() < content_length) {
      if (error.value() == asio::error::eof) return ErrorCallback(error);
      AsyncReadContent(content_length);
    } else {
      FinishedCallback();
    }
  }

  void AsyncReadChunkSize() {
    StartTimer();

    asio::async_read_until(socket_, buffer_, kCrLf, strand_.wrap(std::bind(&HttpStream<SocketType>::ReadChunkSizeCallback,
                                                                           this->shared_from_this(),
                                                                           std::placeholders::_1,
                                                                           std::placeholders::_2)));
  }

  void ReadChunkSizeCallback(const std::error_code &error, size_t bytes_transferred) {
    std::cout << "ReadChunkSizeCallback, error: " << error << ", bytes_transferred: " << bytes_transferred << std::endl;

    if (error && error.value() != asio::error::eof) return ErrorCallback(error);

    size_t chunk_size;
    std::istream is(&buffer_);
    GetChunkSize(is, chunk_size);
    if (!is.good()) return ErrorCallback(std::error_code(HttpStreamErrorCategory::INVALID_CHUNK_SIZE, HttpStreamErrorCategory::category()));

    ChunkInfo chunk_info(chunk_size);

    if (chunk_info.chunk_size() == 0) return AsyncReadEmptyCrlf(true);

    ReadChunkFromBuffer(chunk_info);

    chunk_info ? AsyncReadChunkedContent(chunk_info) : AsyncReadEmptyCrlf(false);
  }

  void ReadChunkFromBuffer(ChunkInfo &chunk_info) {
    if (buffer_.size() > 0) {
      size_t read_bytes = std::min(chunk_info.ChunkBytesLeft(), buffer_.size());

      std::error_code error;
      asio::streambuf::const_buffers_type bufs = buffer_.data();
      data_callback_(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + read_bytes, error);
      buffer_.consume(read_bytes);
      chunk_info.ChunkBytesRead(read_bytes);
    }
  }

  void AsyncReadChunkedContent(const ChunkInfo &chunk_info) {
    StartTimer();

    asio::async_read(socket_, buffer_, strand_.wrap(std::bind(&HttpStream<SocketType>::ReadChunkedContentCallback,
                                                              this->shared_from_this(),
                                                              std::placeholders::_1,
                                                              std::placeholders::_2,
                                                              chunk_info)));
  }

  void ReadChunkedContentCallback(const std::error_code &error, size_t bytes_transferred, ChunkInfo &chunk_info) {
    std::cout << "ReadChunkedContentCallback, error: " << error << ", bytes_transferred: " << bytes_transferred << std::endl;

    if (error && error.value() != asio::error::eof) return ErrorCallback(error);
    if (bytes_transferred == 0) {
      return ErrorCallback(std::error_code(asio::error::eof, asio::error::system_category));
    }

    ReadChunkFromBuffer(chunk_info);

    chunk_info ? AsyncReadChunkedContent(chunk_info) : AsyncReadEmptyCrlf(false);
  }

  void AsyncReadEmptyCrlf(bool last_chunk) {
    StartTimer();

    asio::async_read_until(socket_, buffer_, kCrLf, strand_.wrap(std::bind(&HttpStream<SocketType>::ReadEmptyCrlfCallback,
                                                                           this->shared_from_this(),
                                                                           std::placeholders::_1,
                                                                           std::placeholders::_2,
                                                                           last_chunk)));
  }

  void ReadEmptyCrlfCallback(const std::error_code &error, size_t bytes_transferred, bool last_chunk) {
    std::cout << "ReadEmptyCrlfCallback, error: " << error << ", bytes_transferred: " << bytes_transferred << ", last_chunk: " << last_chunk << std::endl;

    if (error && error.value() != asio::error::eof) return ErrorCallback(error);

    auto r = buffer_.sbumpc();
    auto n = buffer_.sbumpc();

    if (r == EOF || n == EOF || r != '\r' || n != '\n') {
      return ErrorCallback(std::error_code(HttpStreamErrorCategory::MISSING_CHUNK_CRLF, HttpStreamErrorCategory::category()));
    }

    last_chunk ? FinishedCallback() : AsyncReadChunkSize();
  }

  void ErrorCallback(const std::error_code &error) {
    if (!callback_) return;

    timer_.cancel();
    CloseSocket();
    callback_(error, Response());
    callback_ = nullptr;
  }

  void FinishedCallback() {
    if (!callback_) return;

    timer_.cancel();
    CloseSocket();
    callback_(std::error_code(), std::move(response_));
    callback_ = nullptr;
  }

  void StartTimer() {
    timer_.expires_from_now(std::chrono::seconds(5));
    timer_.async_wait(strand_.wrap(std::bind(&HttpStream<SocketType>::TimeoutCallback, this->shared_from_this(), std::placeholders::_1)));
  }

  void TimeoutCallback(const std::error_code &error) {
    if (error != asio::error::operation_aborted) ErrorCallback(std::error_code(asio::error::timed_out, asio::error::get_system_category()));
  }

  void CloseSocket() {
    if (!socket_.is_open()) return;
    std::error_code error;
    socket_.shutdown(asio::ip::tcp::socket::shutdown_both, error);
    assert(!error);
    socket_.close(error);
    assert(!error);
  }

  void MemoryDataCallback(asio::buffers_iterator<asio::streambuf::const_buffers_type> begin, asio::buffers_iterator<asio::streambuf::const_buffers_type> end, std::error_code&) {
    response_.content_.append(begin, end);
  }

  asio::io_service::strand strand_;
  asio::ip::tcp::resolver resolver_;
  SocketType socket_;
  asio::steady_timer timer_;

  asio::streambuf request_buffer_, buffer_;

  Request request_;
  Response response_;

  std::function<void (asio::buffers_iterator<asio::streambuf::const_buffers_type> begin,
                      asio::buffers_iterator<asio::streambuf::const_buffers_type> end,
                      std::error_code &error)> data_callback_;

  std::function<void (const std::error_code&, Response response)> callback_;
};
}

#endif // HTTP_STREAM

