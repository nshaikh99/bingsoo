#ifndef echo_request_handler_h
#define echo_request_handler_h

#include <string>
#include "reply.h"

class Echo_Request_Handler{
    public:
        Echo_Request_Handler();
        reply handleRequest(char* data, size_t bytes_transferred, reply::status_type status);
    private:
        reply reply_;
};

#endif // echo_request_handler_h