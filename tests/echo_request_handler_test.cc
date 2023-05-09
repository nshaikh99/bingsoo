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
  
  bool status_success = response.status == reply::ok;
  bool content_success = response.content == response.content;
  bool content_size_success = response.headers[0].value == std::to_string(response.content.size());
  bool content_type_success = response.headers[1].value == "text/plain";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
} 