#ifndef session_h
#define session_h

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "reply.h"
#include "request_parser.h"
#include "request.h"

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, std::vector<std::string> parsed_config_paths);
  tcp::socket& socket();
  bool start();
  reply generate_response(char *data_, int bytes_transferred, reply::status_type status);
  enum { max_length = 1024 };
  char data_[max_length];
  int handle_read(const boost::system::error_code& error, size_t bytes_transferred);
  std::string request_info();
private:
  void handle_write(const boost::system::error_code& error);
  tcp::socket socket_;
  reply reply_;
  request_parser req_parser_;
  request req;
  std::vector<std::string> parsed_config_paths_;
};

#endif
