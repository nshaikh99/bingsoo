#ifndef echo_request_handler_h
#define echo_request_handler_h

#include <string>
#include "request_handler.h"
#include "reply.h"

class Echo_Request_Handler : public Request_Handler{
    public:
        reply handleRequest(char* data, int bytes_transferred, reply::status_type status);
};

#endif // echo_request_handler_h 
