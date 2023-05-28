#include "gtest/gtest.h"
#include "health_request_handler.h"
#include "reply.h"

class HealthRequestHandlerTestFixture : public ::testing::Test {
  protected:
    HealthHandler handler;
};

TEST_F(HealthRequestHandlerTestFixture, ValidRequest){
  http::request<http::string_body> request;
  request.body() = "OK";
  std::ostringstream request_ostring;
  request_ostring << request.body();
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
  bool content_size_success = fields[0] == std::to_string(request_string.length());
  bool content_type_success = fields[1] == "text/plain";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
} 