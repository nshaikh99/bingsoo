#ifndef markdown_handler_h
#define markdown_handler_h

#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
typedef bool status;

class MarkdownHandler : public RequestHandler {
 public:
  status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
};

#endif // markdown_handler_h
