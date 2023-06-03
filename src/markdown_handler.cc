#include "markdown_handler.h"

status MarkdownHandler::handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res) {
  res.body() = "OK\n";
  res.result(http::status::ok);
  res.content_length(res.body().size());
  res.set(http::field::content_type, "text/plain");
  return true;
}
