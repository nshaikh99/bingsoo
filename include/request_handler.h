#ifndef request_handler_h
#define request_handler_h

#include <boost/beast/http.hpp>
#include "reply.h"

namespace http = boost::beast::http;
typedef bool status;

class RequestHandler { // base class
  public:
    virtual status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res) = 0; 
    // implemented by echo_request_handler, static_request_handler, crud_request_handler, health_handler, sleep_handler
};

#endif //request_handler_h 