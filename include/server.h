#ifndef server_h
#define server_h

#include <boost/asio.hpp>

#include "session.h"

using boost::asio::ip::tcp;

class server
{
<<<<<<< HEAD
public:
  server(boost::asio::io_service& io_service, short port, NginxConfig config);
private:
=======
 public:
  server(boost::asio::io_service& io_service, short port, std::vector<std::string> parsed_config_paths, std::string echo_path, bool is_echo, bool is_static);
 private:
>>>>>>> 187db93 (Code cleanup and additional logging)
  void start_accept();
  void handle_accept(session* new_session, const boost::system::error_code& error);
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  NginxConfig config_;
};

#endif
