#include "crud_handler_factory.h"
#include "crud_request_handler.h"

CrudHandlerFactory::CrudHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* CrudHandlerFactory::create() {
  return new CrudHandler(path_);
}