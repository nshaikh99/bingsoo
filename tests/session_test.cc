#include "gtest/gtest.h"
#include "session.h"
#include <iostream>
using namespace std;


class SessionTestFixture : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    NginxConfigParser config_parser;
    NginxConfig config;
};

TEST_F(SessionTestFixture, StartSession){
  bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
  session* new_session = new session(io_service, config);
  EXPECT_TRUE(new_session->start());
}

TEST_F(SessionTestFixture, ValidRequest)
{
  bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
  session* new_session = new session(io_service, config);
  const char* request = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  strcpy(new_session->data_, request);
  int res = new_session->handle_read(boost::system::error_code(), new_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, InvalidRequest)
{
  bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
  session* new_session = new session(io_service, config);
  const char* request = "fake request";
  strcpy(new_session->data_, request);
  int res = new_session->handle_read(boost::system::error_code(), new_session->max_length);
  EXPECT_EQ(res, 1);
}
