#include "404_handler_factory.h"
#include "404_request_handler.h"

_404HandlerFactory::_404HandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* _404HandlerFactory::create()
{
  return new _404Handler();
}