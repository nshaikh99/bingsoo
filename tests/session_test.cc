#include "gtest/gtest.h"
// #include "session.h"
// // #include "config_parser.h"
// #include "reply.h"
// #include <boost/asio.hpp>
#include "session.h"
#include "request_parser.h"
#include "request.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "global.h"
using namespace std;


class SessionTestFixture : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    // std::vector<std::string> parsed_config_paths;
    // bool is_echo = true;
    // bool is_static = true;
    // std::string echo_path = "/echo";
    NginxConfigParser config_parser;
    NginxConfig config;
  // protected:
  //   boost::asio::io_service io_service;
  //   // std::vector<std::string> parsed_config_paths;
  //   // bool is_echo = true;
  //   // bool is_static = true;
  //   // std::string echo_path = "/echo";
  //   NginxConfigParser config_parser;
  //   NginxConfig config;
    // bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
    // session* new_session = new session(io_service, config);
};

TEST_F(SessionTestFixture, StartSession){
  bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
  session* new_session = new session(io_service, config);
  EXPECT_TRUE(new_session->start());
}

TEST_F(SessionTestFixture, ValidRequest)
{
//   ostringstream data;
//   ostream os(data);
//   string request = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
//   os << request;
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
