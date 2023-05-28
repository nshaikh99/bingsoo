#ifndef sleep_handler_factory_h
#define sleep_handler_factory_h

#include "request_handler_factory.h"

class SleepHandlerFactory : public RequestHandlerFactory
{
  public:
    SleepHandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //sleep_handler_factory_h 