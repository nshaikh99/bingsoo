#ifndef request_handler_factory_h
#define request_handler_factory_h

#include "request_handler.h"
#include "config_parser.h"

class RequestHandlerFactory
{
  public:
    virtual RequestHandler* create() = 0;
};

#endif //request_handler_factory_h 