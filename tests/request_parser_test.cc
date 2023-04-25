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


TEST_F(RequestParserTestFixture, InvalidMethodStart) {
  char input[1024] = "!ET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, StartMethodWithSpecialChar) {
  char input[1024] = "(ET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, DigitAppearingBeforeHTTP) {
  char input[1024] = "GET /0HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidMethod) {
  char input[1024] = "0ET./ FAKE/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidSecondLetterOfHTTP) {
  char input[1024] = "GET / HFTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidThirdLetterOfHTTP) {
  char input[1024] = "GET / HTLP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidFourthLetterOfHTTP) {
  char input[1024] = "GET / HTTZ/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, NoSlashAfterHTTP) {
  char input[1024] = "GET / HTTP.1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, NoDigitAfterSlash) {
  char input[1024] = "GET / HTTP/M.1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, NoPeriodAfterMajorStart) {
  char input[1024] = "GET / HTTP/1,1\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, DigitAfterMajorStart) {
  char input[1024] = "GET / HTTP/121\r\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, InvalidHTTPMinor) {
  char input[1024] = "GET / HTTP/1.M\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, DigitHTTPMinor) {
  char input[1024] = "GET / HTTP/1.12\nHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, NoNewLine) {
  char input[1024] = "GET / HTTP/1.1\rXHost: www.test.com\r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, StartHeaderWithSpace) {
  char input[1024] = "GET / HTTP/1.1\r\n \r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, StartHeaderWithSpecialChar) {
  char input[1024] = "GET / HTTP/1.1\r\n{ \r\nConnection: close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, SpecialCharacterForHeaderName) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection( close\r\n\r";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, NoSpaceBeforeHeaderValue) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection:close\r\n\r\n";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

// CTL: a number between 0 and 31, and 127 inclusive 
TEST_F(RequestParserTestFixture, CTLforHeaderValue) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: 0";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);
  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, MissingThirdNewline) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\r\n";
  auto pair = req_parser.parse(req, input, input + strlen(input));
  res = std::get<0>(pair);

  bool failure = res == request_parser::good ? true : false;
  
  EXPECT_FALSE(failure);
}

TEST_F(RequestParserTestFixture, Reset) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  req_parser.reset();
  auto pair = req_parser.parse(req, input, input + strlen(input));

  res = std::get<0>(pair);
  
  bool success = res == request_parser::good ? true : false;
  
  EXPECT_TRUE(success);
}

