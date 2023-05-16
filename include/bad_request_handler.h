#ifndef bad_request_handler_h
#define bad_request_handler_h

#include <string>
#include "request_handler.h"
#include "reply.h"
#include <boost/beast/http.hpp>
 
namespace http = boost::beast::http;
typedef bool status;

class BadHandler : public RequestHandler{
  public:
    status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif // bad_request_handler_h 