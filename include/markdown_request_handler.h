#ifndef markdown_request_handler_h
#define markdown_request_handler_h

#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
typedef bool status;

class MarkdownHandler : public RequestHandler {
 public:
  MarkdownHandler(std::string data_path = "./");
  status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
 private:
  std::string data_path_;
};

#endif // markdown_handler_h
