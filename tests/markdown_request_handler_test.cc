#include "gtest/gtest.h"
#include "markdown_request_handler.h"
#include "reply.h"
#include <fstream>

class MarkdownRequestHandlerTestFixture : public ::testing::Test {};

TEST_F(MarkdownRequestHandlerTestFixture, SampleMDRequest){
  std::string filename = "../markdown";
  MarkdownHandler test_handler = MarkdownHandler(filename);
  http::response<http::string_body> test_res;
  http::request<http::string_body> test_req;
  test_req.target("/markdown/sample.md");
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
  bool content_type_success = fields[1] == "text/html";
  EXPECT_TRUE(status_success);
  EXPECT_TRUE(content_type_success);
}

TEST_F(MarkdownRequestHandlerTestFixture, NotanMDRequest){
  std::string filename = "../markdown";
  MarkdownHandler test_handler = MarkdownHandler(filename);
  http::response<http::string_body> test_res;
  http::request<http::string_body> test_req;
  test_req.target("/markdown/sample.html");
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

  bool status_success = test_res.result() == http::status::bad_request;
  bool content_type_success = fields[1] == "text/plain";
  EXPECT_TRUE(status_success);
  EXPECT_TRUE(content_type_success);
}