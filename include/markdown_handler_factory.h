#ifndef markdown_handler_factory_h
#define markdown_handler_factory_h

#include "request_handler_factory.h"

class MarkdownHandlerFactory : public RequestHandlerFactory
{
  public:
    MarkdownHandlerFactory(const std::string& location_path_, const NginxConfig& config);
    RequestHandler* create();
  private:
    std::string location_path_;
    std::string data_path_;
    NginxConfig config_;
};

#endif //markdown_handler_factory_h 