#ifndef bad_handler_factory_h
#define bad_handler_factory_h

#include "request_handler_factory.h"

class BadHandlerFactory : public RequestHandlerFactory
{
  public:
    BadHandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //bad_handler_factory_h 