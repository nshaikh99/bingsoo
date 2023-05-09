#ifndef request_handler_h
#define request_handler_h

#include "reply.h"
#include "request.h"

<<<<<<< HEAD
class Request_Handler {
    public:
        virtual reply handleRequest(char* data, int bytes_transferred) = 0;
    protected:
        reply reply_;
=======
class Request_Handler // base class
{
 public:
  virtual reply handleRequest(char* data, int bytes_transferred, reply::status_type status) = 0; // implemented by echo_request_handler and static_request_handler
 protected:
  reply reply_;
>>>>>>> 187db93 (Code cleanup and additional logging)
};

#endif
