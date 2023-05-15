#ifndef _404_request_handler_h
#define _404_request_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>
 
using namespace boost::beast::http;

class _404Handler : public RequestHandler{
  public:
    status handle_request(const request req, response& res);
}

#endif // _404_request_handler_h