#ifndef crud_handler_factory_h
#define crud_handler_factory_h

#include "request_handler_factory.h"

class CrudHandlerFactory : public RequestHandlerFactory
{
  public:
    CrudHandlerFactory(const std::string& path, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string path_;
    NginxConfig config_;
};

#endif //crud_handler_factory_h