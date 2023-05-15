#ifndef request_handler_h
#define request_handler_h

#include <boost/beast/http.hpp>

using namespace boost::beast::http;

class RequestHandler { // base class
    public:
        virtual status handle_request(const request req, response& res) = 0; // implemented by echo_request_handler and static_request_handler
};

#endif //request_handler_h 
