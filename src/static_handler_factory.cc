#include "static_handler_factory.h"
#include "static_request_handler.h"

StaticHandlerFactory::StaticHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* StaticHandlerFactory::create()
{
  return new StaticHandler(path_);
}