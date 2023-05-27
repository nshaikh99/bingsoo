#ifndef health_handler_factory_h
#define health_handler_factory_h

#include "request_handler_factory.h"

class HealthHandlerFactory : public RequestHandlerFactory
{
  public:
    HealthHandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //health_handler_factory_h 