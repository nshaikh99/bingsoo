#include "gtest/gtest.h"
#include "session.h"
#include "echo_handler_factory.h"
#include <iostream>
#include <unordered_map>
#include <strstream>
using namespace std;
 
class SessionTestFixture : public ::testing::Test {
  protected:
    NginxConfigParser config_parser;
    NginxConfig config;
    boost::asio::io_service io_service; 
    std::unordered_map<std::string, RequestHandlerFactory*> routes;
    void SetUp() override 
    {
      istrstream config_file("../conf/sample.conf");
      std::istream* config_path = &config_file;
      config_parser.Parse(config_path, &config);
    }
};

TEST_F(SessionTestFixture, StartSession){
  session* test_session = new session(io_service, config, routes);
  EXPECT_TRUE(test_session->start());
}

TEST_F(SessionTestFixture, ValidRequest)
{
  routes["/echo"] = new EchoHandlerFactory("/echo", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET /echo HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
}