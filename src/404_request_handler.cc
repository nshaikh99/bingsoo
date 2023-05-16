#include "404_request_handler.h"

status _404Handler::handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res){
    res.body() = stock_replies::not_found;
    res.result(http::status::not_found);
    res.content_length((res.body().size()));
    res.set(http::field::content_type, "text/html");
    return true;
}