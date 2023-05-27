#include "health_request_handler.h"

status HealthHandler::handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res){
  res.body() = "OK";
  res.result(http::status::ok);
  res.content_length((res.body().size()));
  res.set(http::field::content_type, "text/plain");
  return true;
}
