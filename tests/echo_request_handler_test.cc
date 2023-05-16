#include "gtest/gtest.h"
#include "echo_request_handler.h"
#include "reply.h"

class EchoRequestHandlerTestFixture : public ::testing::Test {
  protected:
    EchoHandler handler;
};

TEST_F(EchoRequestHandlerTestFixture, ValidRequest){
  http::request<http::string_body> request;
  request.target("/echo");
  request.body() = "sample request";
  std::ostringstream request_ostring;
  request_ostring << request;
  std::string request_string = request_ostring.str();

  http::response<http::string_body> response;
  handler.handle_request(request, response);

  int idx = 0;
  std::string fields[2];
  for (auto& field : response.base()) {
    fields[idx] = std::string(field.value());
    idx++;
  }
  
  bool status_success = response.result() == http::status::ok;
  bool content_success = response.body() == request_string;
  bool content_size_success = fields[1] == std::to_string(request_string.length());
  bool content_type_success = fields[0] == "text/plain";
  EXPECT_TRUE(status_success);
} 