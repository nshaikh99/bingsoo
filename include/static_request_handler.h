#ifndef static_request_handler_h
#define static_request_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

using namespace boost::beast::http;

class StaticHandler : public RequestHandler{
  public:
    StaticHandler(std::string path);
    status handle_request(const request req, response& res);
  private:
    std::string file_path_;
};

#endif // static_request_handler_h