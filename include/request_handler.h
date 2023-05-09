#ifndef request_handler_h
#define request_handler_h

#include "request.h"
#include "reply.h"

class Request_Handler { // base class
    public:
        virtual reply handleRequest(char* data, int bytes_transferred) = 0; // implemented by echo_request_handler and static_request_handler
    protected:
        reply reply_;
};

#endif //request_handler_h 
