#ifndef crud_request_handler_h
#define crud_request_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
typedef bool status;

class CrudHandler : public RequestHandler {
  public:
    CrudHandler(std::string path);
    status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
  private:
    std::string data_path_;
};

#endif // crud_request_handler_h