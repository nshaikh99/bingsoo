#include "gtest/gtest.h"
#include "session.h"
#include "reply.h"
#include <boost/asio.hpp>

class ReplyTestFixture : public ::testing::Test {
  protected:
    reply rep;
    boost::asio::io_service io_service;
    std::vector<std::string> parsed_config_paths;
    bool is_echo = true;
    bool is_static = true;
    std::string echo_path = "/echo";
    session* new_session = new session(io_service, parsed_config_paths, echo_path, is_echo, is_static);
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

