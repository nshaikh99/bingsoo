#ifndef server_h
#define server_h

#include "session.h"
#include "request_handler_factory.h"
#include "404_handler_factory.h"
#include "echo_handler_factory.h"
#include "static_handler_factory.h"

#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;
using namespace std;

class server
{
public:
  server(boost::asio::io_service& io_service, short port, NginxConfig config);
  RequestHandlerFactory* createHandlerFactory(string name, string uri);
  std::unordered_map<std::string, RequestHandlerFactory*> configMap();
private:
  void start_accept();
  void handle_accept(session* new_session, const boost::system::error_code& error);
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  NginxConfig config_;
};

#endif
