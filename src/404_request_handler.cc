#include "404_request_handler.h"
#include "reply.h"

status _404Handler::handleRequest(const request req, response& res){
    beast::ostream(res.body()) << reply_.stock_reply(reply::not_found);
    res.result(http::status::not_found);
    res.content_length((res.body().size()));
    res.set(http::field::content_type, "text/html");
    return res.result();
}