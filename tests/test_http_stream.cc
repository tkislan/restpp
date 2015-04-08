#include "gtest/gtest.h"

#include "http_stream.h"
#include "request.h"

#include "mocks/mock_server.h"

using namespace restpp;

TEST(TestHttpStream, Simple) {
  MockServer server(9000);
  server.set_response("HTTP/1.1\r\n\r\n");
  server.set_response("HTTP/1.1 200 OK\r\nX-header: some value\r\n\r\n");

  server.Listen();

  asio::io_service io_service;

  Request req;
  req.set_host("localhost");
  req.set_port(9000);

  req.set_method(HttpMethod::GET);
  req.set_path("/api");
  req.add_header("Host", "localhost:8000");
  req.add_header("Connection", "close");

  asio::ip::tcp::socket socket(io_service);

  auto http = std::make_shared<HttpStream<asio::ip::tcp::socket>>(io_service, std::move(socket), std::move(req), [](const std::error_code &error, Response resp) {
    std::cout << "Finished: " << error.message() << std::endl;

//    ASSERT_EQ(std::error_code(HttpStreamErrorCategory::INVALID_STATUS_LINE, HttpStreamErrorCategory::category()), error);
    ASSERT_EQ(std::error_code(), error);

    std::cout << "Request successful: " << resp.status() << std::endl;
    std::cout << "Request successful: " << resp.status_message() << std::endl;
    std::cout << "------ Content ------" << std::endl;
    std::cout << resp.content() << std::endl;
  });

  http->Run();

  io_service.run();

  std::cout << "Http finished" << std::endl;

  server.Stop();

  std::cout << "Server finished" << std::endl;
}
