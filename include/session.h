#ifndef session_h
#define session_h

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "reply.h"
#include "request_parser.h"
#include "request.h"
#include "config_parser.h"
#include "404_request_handler.h"
#include "bad_request_handler.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "request_handler_factory.h"
#include "404_handler_factory.h"
#include "bad_handler_factory.h"
#include "echo_handler_factory.h"
#include "static_handler_factory.h"

#include <boost/beast/http.hpp>

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class session
{
public:
  session(boost::asio::io_service& io_service, NginxConfig config, std::unordered_map<std::string, RequestHandlerFactory*> routes);
  tcp::socket& socket();
  bool start();
  enum { max_length = 1024 };
  char data_[max_length];
  int handle_read(const boost::system::error_code& error, size_t bytes_transferred);
  std::string request_info();
  reply::request_type get_request_type();
private:
  void handle_write(const boost::system::error_code& error);
  tcp::socket socket_;
  reply reply_;
  request_parser req_parser_;
  NginxConfig config_;
  request req_;
  http::request<http::string_body> request_;
  http::response<http::string_body> response_;
  std::unordered_map<std::string, RequestHandlerFactory*> routes_;
};

#endif
