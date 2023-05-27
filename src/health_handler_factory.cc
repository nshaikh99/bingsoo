#include "health_handler_factory.h"
#include "health_request_handler.h"

HealthHandlerFactory::HealthHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* HealthHandlerFactory::create()
{
  return new HealthHandler();
}