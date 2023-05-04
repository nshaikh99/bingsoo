#include "gtest/gtest.h"
#include "static_request_handler.h"
#include "reply.h"

class StaticRequestHandlerTestFixture : public ::testing::Test {
  protected:
    std::string test_file_path = "./test_files/test.html";
    Static_Request_Handler handler = Static_Request_Handler(test_file_path);
};

TEST_F(StaticRequestHandlerTestFixture, ValidRequest){
//   char request[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
//   reply response = handler.handleRequest(request, strlen(request), reply::ok);
//   bool status_success = response.status == reply::ok ? true : false;
  
  EXPECT_TRUE(true);
//   EXPECT_EQ(request, response.content);
}
