#include "gtest/gtest.h"
#include "session.h"
#include <iostream>
using namespace std;

<<<<<<< HEAD

class SessionTestFixture : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    NginxConfigParser config_parser;
    NginxConfig config;
};

TEST_F(SessionTestFixture, StartSession){
  bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
  session* new_session = new session(io_service, config);
=======
class SessionTestFixture : public ::testing::Test
{
 protected:
  boost::asio::io_service io_service;
  std::vector<std::string> parsed_config_paths;
  std::string echo_path = "/echo";
  bool is_echo = true;
  bool is_static = true;
  session* new_session = new session(io_service, parsed_config_paths, echo_path, is_echo, is_static);
};

TEST_F(SessionTestFixture, StartSession)
{
>>>>>>> 187db93 (Code cleanup and additional logging)
  EXPECT_TRUE(new_session->start());
}

TEST_F(SessionTestFixture, ValidRequest)
{
<<<<<<< HEAD
  bool parsed_config = config_parser.Parse("./tests/test_configs/static_echo_config", &config);
  session* new_session = new session(io_service, config);
=======
>>>>>>> 187db93 (Code cleanup and additional logging)
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
