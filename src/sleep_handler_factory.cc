#include "sleep_handler_factory.h"
#include "sleep_request_handler.h"

SleepHandlerFactory::SleepHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* SleepHandlerFactory::create()
{
  return new SleepHandler();
}