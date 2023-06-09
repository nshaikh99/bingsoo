#include "gtest/gtest.h"
#include "server.h"
#include <strstream>

class ServerTestFixture : public ::testing::Test
{
  protected:
    NginxConfigParser config_parser;
    NginxConfig config;
    boost::asio::io_service io_service; 
    void SetUp() override 
    {
      istrstream config_file("../conf/sample.conf");
      std::istream* config_path = &config_file;
      config_parser.Parse(config_path, &config);
    }
};

TEST_F(ServerTestFixture, StartAccept)
{ 
  server test_server(io_service, 8080, config);
  bool success = test_server.start_accept();
  EXPECT_TRUE(success);
}

TEST_F(ServerTestFixture, HandleAccept)
{ 
  server test_server(io_service, 8080, config);
  std::unordered_map<std::string, RequestHandlerFactory*> routes = test_server.configMap();
  session* test_session = new session(io_service, config, routes);
  bool success = test_server.handle_accept(test_session, boost::system::error_code());
  EXPECT_TRUE(success);
}

TEST_F(ServerTestFixture, ConfigMap)
{
  server test_server(io_service, 8080, config);
  std::unordered_map<std::string, RequestHandlerFactory*> routes = test_server.configMap();
  std::string echo_path = config.get_echo_path(); 
  std::string crud_path = config.get_crud_path();
  std::string health_path = config.get_health_path();
  std::string sleep_path = config.get_sleep_path();
  std::string markdown_path = config.get_markdown_path();
  std::unordered_map<std::string, std::string> static_path_map = config.get_static_file_path();
  test_server.createHandlerFactory("StaticHandler", "/static");
  bool static_success = true;
  for (auto it : static_path_map){
    if (routes[it.first] == nullptr){
        static_success = false;
    }
  }
  bool echo_success = routes[echo_path] != nullptr;
  bool bad_success = routes["bad"] != nullptr;
  bool crud_success = routes[crud_path] != nullptr;
  bool health_success = routes[health_path] != nullptr;
  bool sleep_success = routes[sleep_path] != nullptr;
  bool _404_success = routes["/"] != nullptr;
  bool markdown_success = routes[markdown_path] != nullptr;
  EXPECT_TRUE(static_success && echo_success && bad_success && crud_success && health_success && sleep_success && _404_success && markdown_success);
}
 
