#ifndef server_h
#define server_h

#include "session.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class server
{
public:
  server(boost::asio::io_service& io_service, short port, NginxConfig config);
private:
  void start_accept();
  void handle_accept(session* new_session, const boost::system::error_code& error);
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  NginxConfig config_;
};

#endif
