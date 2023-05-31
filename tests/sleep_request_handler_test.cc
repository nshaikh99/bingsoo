#include "gtest/gtest.h"
#include "sleep_request_handler.h"
#include "reply.h"

class SleepRequestHandlerTestFixture : public ::testing::Test {
  protected:
    SleepHandler handler;
};

TEST_F(SleepRequestHandlerTestFixture, ValidRequest){
  http::request<http::string_body> request;
  request.body() = "sample request";
  std::ostringstream request_ostring;
  request_ostring << request.body();
  std::string request_string = request_ostring.str();

  http::response<http::string_body> response;
  handler.handle_request(request, response);
  std::string request_content = "Slept for 2 seconds\n";

  // Populate an array of strings with the value from the HTTP response so we
  // can verify these strings match the expected response
  int idx = 0;
  std::string fields[2];
  for (auto& field : response.base()) {
    fields[idx] = std::string(field.value());
    idx++;
  }
  
  bool status_success = response.result() == http::status::ok;
  bool content_success = response.body() == request_content;
  bool content_size_success = fields[0] == std::to_string(request_content.length());
  bool content_type_success = fields[1] == "text/plain";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
} 