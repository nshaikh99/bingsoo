#include "gtest/gtest.h"
#include "session.h"
#include "reply.h"
#include <boost/asio.hpp>

class ReplyTestFixture : public ::testing::Test {
  protected:
    reply rep;
    boost::asio::io_service io_service;
    NginxConfigParser config_parser;
    NginxConfig config;
    bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
    session* new_session = new session(io_service, config);
};

TEST_F(ReplyTestFixture, ValidRequest) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  rep = new_session->generate_response(input, strlen(input), reply::ok);
  bool status_success = rep.status == reply::ok ? true : false;
  
  EXPECT_TRUE(status_success);
  EXPECT_EQ(input, rep.content);
}

TEST_F(ReplyTestFixture, BadRequest) {
  char input[1024] = "fake request";
  rep = new_session->generate_response(input, strlen(input), reply::bad_request);
  bool status_failure = rep.status == reply::bad_request ? true : false;
  
  EXPECT_TRUE(status_failure);
  EXPECT_EQ(input, rep.content);
}

