#include "bad_handler_factory.h"
#include "bad_request_handler.h"

BadHandlerFactory::BadHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* BadHandlerFactory::create()
{
  return new BadHandler();
}