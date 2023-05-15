#include "echo_request_handler.h"

status EchoHandler::handleRequest(const request req, response& res){
    beast::ostream(res.body()) << req;
    res.result(http::status::ok);
    res.content_length((res.body().size()));
    res.set(http::field::content_type, "text/plain");
}