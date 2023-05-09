#include "gtest/gtest.h"
#include "echo_request_handler.h"
#include "reply.h"

class EchoRequestHandlerTestFixture : public ::testing::Test {
  protected:
    Echo_Request_Handler handler;
};

TEST_F(EchoRequestHandlerTestFixture, ValidRequest){
  char request[1024] = "sample request";
  reply response = handler.handleRequest(request, strlen(request));
  
  bool success1 = response.status == reply::ok;
  bool success2 = response.content == response.content;
  bool success3 = response.headers[0].value == std::to_string(response.content.size());
  bool success4 = response.headers[1].value == "text/plain";
  EXPECT_TRUE(success1 && success2 && success3 && success4);
} 