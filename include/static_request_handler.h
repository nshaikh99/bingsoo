#ifndef static_request_handler_h
#define static_request_handler_h

#include <string>
#include "request_handler.h"
#include "reply.h"

class Static_Request_Handler : public Request_Handler{
    public:
        Static_Request_Handler(std::string file_path);
        reply handleRequest(char* data, int bytes_transferred);
    private:
        std::string file_path_;
};

#endif // static_request_handler_h