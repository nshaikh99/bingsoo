#include "gtest/gtest.h"
#include "404_request_handler.h"
#include <fstream>

using namespace std;

class _404RequestHandlerTestFixture : public ::testing::Test {};

TEST_F(_404RequestHandlerTestFixture, _404Request){
  _404Handler handler = _404Handler();
  http::response<http::string_body> test_res;
  http::request<http::string_body> test_req;
  handler.handle_request(test_req, test_res);

  int idx = 0;
  std::string fields[2];
  for (auto& field : test_res.base()) {
    fields[idx] = std::string(field.value());
    idx++;
  }

  bool status_success = test_res.result() == http::status::not_found;
  bool content_success = test_res.body() == stock_replies::not_found;
  bool content_size_success = fields[0] == std::to_string(test_res.body().size());
  bool content_type_success = fields[1] == "text/html";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
}