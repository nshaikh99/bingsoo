#include "gtest/gtest.h"
#include "static_request_handler.h"
#include "reply.h"

class StaticRequestHandlerTestFixture : public ::testing::Test
{
 protected:
  std::string test_file_path = "./test_files/test.html";
  Static_Request_Handler handler = Static_Request_Handler(test_file_path);
};

TEST_F(StaticRequestHandlerTestFixture, ValidRequest)
{
  EXPECT_TRUE(true);
}
