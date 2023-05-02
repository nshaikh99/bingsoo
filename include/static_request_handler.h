#ifndef static_request_handler_h
#define static_request_handler_h

#include <string>
#include "reply.h"

class Static_Request_Handler{
    public:
        Static_Request_Handler(std::string file_path);
        reply getStaticReply(char* data, size_t bytes_transferred, reply::status_type status);
    private:
        std::string file_path_;
        reply reply_;
};

#endif // static_request_handler_h