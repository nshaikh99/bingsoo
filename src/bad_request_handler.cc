#include "bad_request_handler.h"

status BadHandler::handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res){
    res.body() = stock_replies::bad_request;
    res.result(http::status::bad_request);
    res.content_length((res.body().size()));
    res.set(http::field::content_type, "text/html");
    return true;
} 