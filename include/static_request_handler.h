#ifndef static_request_handler_h
#define static_request_handler_h

#include <string>

#include "request_handler.h"
#include "reply.h"

<<<<<<< HEAD
class Static_Request_Handler : public Request_Handler{
    public:
        Static_Request_Handler(std::string file_path);
        reply handleRequest(char* data, int bytes_transferred);
    private:
        std::string file_path_;
=======
class Static_Request_Handler : public Request_Handler
{
 public:
  Static_Request_Handler(std::string file_path);
  reply handleRequest(char* data, int bytes_transferred, reply::status_type status);
 private:
  std::string file_path_;
>>>>>>> 187db93 (Code cleanup and additional logging)
};

#endif
