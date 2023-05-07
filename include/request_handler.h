#ifndef request_handler_h
#define request_handler_h

#include "request.h"
#include "reply.h"

class Request_Handler {
    public:
        virtual reply handleRequest(char* data, int bytes_transferred) = 0;
    protected:
        reply reply_;
};

#endif //request_handler_h 
