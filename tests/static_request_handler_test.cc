#include "gtest/gtest.h"
#include "static_request_handler.h"
#include <fstream>

using namespace std;

class StaticRequestHandlerTestFixture : public ::testing::Test {};

TEST_F(StaticRequestHandlerTestFixture, ValidRequest){
  std::string filename = "../static/index.html";
  StaticHandler test_handler = StaticHandler(filename);
  http::response<http::string_body> test_res;
  http::request<http::string_body> test_req;
  test_handler.handle_request(test_req, test_res);

  std::ifstream test_file(filename.c_str(), std::ios::in | std::ios::binary);

  char c;
  std::string file_content = "";
  while (test_file.get(c))
    file_content += c;
  test_file.close();

  int idx = 0;
  std::string fields[2];
  for (auto& field : test_res.base()) {
    fields[idx] = std::string(field.value());
    idx++;
  }

  bool status_success = test_res.result() == http::status::ok;
  bool content_success = test_res.body() == file_content;
  bool content_size_success = fields[0] == std::to_string(file_content.length());
  bool content_type_success = fields[1] == "text/html";
  EXPECT_TRUE(status_success);
  EXPECT_TRUE(content_success);
  EXPECT_TRUE(content_size_success);
  EXPECT_TRUE(content_type_success);
}

TEST_F(StaticRequestHandlerTestFixture, InvalidRequest){
  std::string filename = "nonexistant.html";
  StaticHandler test_handler = StaticHandler(filename);
  http::response<http::string_body> test_res;
  http::request<http::string_body> test_req;
  bool handled = test_handler.handle_request(test_req, test_res);

  int idx = 0;
  std::string fields[2];
  for (auto& field : test_res.base()) {
    fields[idx] = std::string(field.value());
    idx++;
  }

  bool status_invalid = test_res.result() == http::status::bad_request;
  bool content_invalid = test_res.body() == stock_replies::bad_request;
  bool content_size_invalid = fields[0] == std::to_string(test_res.body().size());
  bool content_type_invalid = fields[1] == "text/html";
  EXPECT_TRUE(status_invalid && content_invalid && content_size_invalid && content_type_invalid);
  EXPECT_FALSE(handled);
}