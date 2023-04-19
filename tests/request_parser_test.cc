#include "gtest/gtest.h"
#include "request_parser.h"
#include "request.h"

#include <iostream>

class RequestParserTestFixture : public ::testing::Test {
  protected:
    request_parser req_parser;
    request req;
    request_parser::result_type res;
};

TEST_F(RequestParserTestFixture, ValidRequest) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool success = res == request_parser::good ? true : false;
  
  EXPECT_TRUE(success);
}

TEST_F(RequestParserTestFixture, InvalidEnding) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidProtocol) {
  char input[1024] = "GET / FAKE/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidVersion) {
  char input[1024] = "GET / HTTP/1.X\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}
