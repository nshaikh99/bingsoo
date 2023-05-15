#ifndef echo_request_handler_h
#define echo_request_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

using namespace boost::beast::http;

class EchoHandler : public RequestHandler{
  public:
    status handle_request(const request req, response& res);
};

#endif // echo_request_handler_h 
