#ifndef static_handler_factory_h
#define static_handler_factory_h

#include "request_handler_factory.h"
#include <string>

class StaticHandlerFactory : public RequestHandlerFactory
{
  public:
    StaticHandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //static_handler_factory_h 