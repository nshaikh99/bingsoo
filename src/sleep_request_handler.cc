#include "sleep_request_handler.h"
#include <unistd.h>

status SleepHandler::handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res){
  sleep(2);
  res.body() = "Slept for 2 seconds\n";
  res.result(http::status::ok);
  res.content_length((res.body().size()));
  res.set(http::field::content_type, "text/plain");
  return true;
}
 