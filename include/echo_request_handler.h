#ifndef echo_request_handler_h
#define echo_request_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
typedef bool status;

class EchoHandler : public RequestHandler{
  public:
    status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif // echo_request_handler_h 