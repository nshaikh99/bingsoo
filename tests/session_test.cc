#include "gtest/gtest.h"
#include "session.h"
#include "reply.h"
#include <boost/asio.hpp>

using namespace std;

class SessionTestFixture : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    std::vector<std::string> parsed_config_paths;
    bool is_echo = true;
    bool is_static = true;
    std::string echo_path = "/echo";
    session* new_session = new session(io_service, parsed_config_paths, echo_path, is_echo, is_static);
};

TEST_F(SessionTestFixture, StartSession){
  EXPECT_TRUE(new_session->start());
}

TEST_F(SessionTestFixture, ValidRequest)
{
//   ostringstream data;
//   ostream os(data);
//   string request = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
//   os << request;
  const char* request = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  strcpy(new_session->data_, request);
  int res = new_session->handle_read(boost::system::error_code(), new_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, InvalidRequest)
{
  const char* request = "fake request";
  strcpy(new_session->data_, request);
  int res = new_session->handle_read(boost::system::error_code(), new_session->max_length);
  EXPECT_EQ(res, 1);
}
