#include "gtest/gtest.h"
#include "bad_request_handler.h"
#include "reply.h"
#include <fstream>

using namespace std;

class BadRequestHandlerTestFixture : public ::testing::Test {
    protected:
        BadHandler handler;
};

TEST_F(BadRequestHandlerTestFixture, Return400BadRequestPage) {
  http::request<http::string_body> request;
  request.body() = "<html><head><title>Not Found</title></head><body><h1>400 Bad Request</h1></body></html>";
  std::ostringstream request_ostring;
  request_ostring << request.body();
  std::string request_string = request_ostring.str();

  http::response<http::string_body> response;
  handler.handle_request(request, response);

  // Populate an array of strings with the value from the HTTP response so we
  // can verify these strings match the expected response
  int idx = 0;
  std::string fields[2];
  for (auto& field : response.base()) {
    fields[idx] = std::string(field.value());
    idx++;
  }
  
  bool status_success = response.result() == http::status::bad_request;
  bool content_success = response.body() == stock_replies::bad_request;
  bool content_size_success = fields[0] == std::to_string(response.body().size());
  bool content_type_success = fields[1] == "text/html";
  EXPECT_TRUE(status_success);
  EXPECT_TRUE(content_success);
  EXPECT_TRUE(content_size_success);
  EXPECT_TRUE(content_type_success);

}