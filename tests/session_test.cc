#include "gtest/gtest.h"
#include "session.h"
#include "reply.h"
#include "request_parser.h"
#include "echo_handler_factory.h"
#include "static_handler_factory.h"
#include "health_handler_factory.h"
#include "sleep_handler_factory.h"
#include "404_handler_factory.h"
#include "crud_handler_factory.h"
#include "markdown_handler_factory.h"
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
    request_parser req_parser_;
    request req_;
    reply::request_type reply_;
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

TEST_F(SessionTestFixture, InvalidRequest)
{
  routes["bad"] = new BadHandlerFactory("bad", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "bad request";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 1);
}

TEST_F(SessionTestFixture, StaticRequest)
{
  routes["/static"] = new StaticHandlerFactory("/static", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET /static HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, HealthRequest)
{
  routes["/health"] = new HealthHandlerFactory("/health", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET /health HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, SleepRequest)
{
  routes["/sleep"] = new SleepHandlerFactory("/sleep", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET /sleep HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, _404Request)
{
  routes["/"] = new _404HandlerFactory("/", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, CrudRequest)
{
  routes["/api"] = new CrudHandlerFactory("/api", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET /api HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
}

TEST_F(SessionTestFixture, MarkdownRequest)
{
  routes["/markdown"] = new MarkdownHandlerFactory("/markdown", config);
  session* test_session = new session(io_service, config, routes);
  const char* request = "GET /markdown HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
  strcpy(test_session->data_, request);
  int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
  EXPECT_EQ(res, 0);
} 

// TEST_F(SessionTestFixture, GetRequestType){
//   routes["/echo"] = new EchoHandlerFactory("/echo", config);
//   session* test_session = new session(io_service, config, routes);
//   const char* request = "GET /echo HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n";
//   strcpy(test_session->data_, request);
//   int res = test_session->handle_read(boost::system::error_code(), test_session->max_length);
//   auto pair = req_parser_.parse(req_, test_session->data_, test_session->data_ + test_session->max_length);
//   req_.uri = "/echo";
//   reply_ = test_session->get_request_type(req_, config);
//   EXPECT_TRUE(true);
// }
