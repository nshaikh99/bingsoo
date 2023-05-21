#include "crud_handler_factory.h"
#include "crud_request_handler.h"

CrudHandlerFactory::CrudHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {
  std::unordered_map<std::string,std::string> args_map = config_.get_crud_args();
  data_path_ = args_map["data_path"];
}

RequestHandler* CrudHandlerFactory::create() {
  return new CrudHandler(data_path_);
}