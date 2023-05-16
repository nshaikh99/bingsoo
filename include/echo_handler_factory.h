#ifndef echo_handler_factory_h
#define echo_handler_factory_h

#include "request_handler_factory.h"

class EchoHandlerFactory : public RequestHandlerFactory
{
  public:
    EchoHandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //echo_handler_factory_h 