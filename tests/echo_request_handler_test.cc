#include "gtest/gtest.h"
#include "echo_request_handler.h"
#include "reply.h"

class EchoRequestHandlerTestFixture : public ::testing::Test {
  protected:
    Echo_Request_Handler handler;
};

TEST_F(EchoRequestHandlerTestFixture, ValidRequest){
  char request[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  reply response = handler.handleRequest(request, strlen(request));
  bool status_success = response.status == reply::ok ? true : false;
  
  EXPECT_TRUE(status_success);
  EXPECT_EQ(request, response.content);
}