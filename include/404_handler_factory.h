#ifndef _404_handler_factory_h
#define _404_handler_factory_h

#include "request_handler_factory.h"

class _404HandlerFactory : public RequestHandlerFactory
{
  public:
    _404HandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //_404_handler_factory_h 