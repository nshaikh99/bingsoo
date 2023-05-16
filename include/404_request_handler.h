#ifndef _404_request_handler_h
#define _404_request_handler_h

#include <string>
#include "request_handler.h"
#include "reply.h"
#include <boost/beast/http.hpp>
 
namespace http = boost::beast::http;
typedef bool status;

class _404Handler : public RequestHandler{
  public:
    status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif // _404_request_handler_h