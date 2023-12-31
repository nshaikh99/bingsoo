#ifndef static_request_handler_h
#define static_request_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
typedef bool status;

class StaticHandler : public RequestHandler{
  public:
    StaticHandler(std::string path);
    status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
  private:
    std::string file_path_;
};
 
#endif // static_request_handler_h