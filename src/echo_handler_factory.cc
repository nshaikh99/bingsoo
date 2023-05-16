#include "echo_handler_factory.h"
#include "echo_request_handler.h"

EchoHandlerFactory::EchoHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* EchoHandlerFactory::create()
{
  return new EchoHandler();
}