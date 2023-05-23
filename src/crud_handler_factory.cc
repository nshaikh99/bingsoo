#include <memory>
#include "crud_handler_factory.h"
#include "crud_request_handler.h"
#include "real_filesystem.h"

CrudHandlerFactory::CrudHandlerFactory(const std::string& path, const NginxConfig& config)
: location_path_(path), config_(config) {
  std::unordered_map<std::string,std::string> args_map = config_.get_crud_args();
  data_path_ = args_map["data_path"];
}

RequestHandler* CrudHandlerFactory::create() {
  std::shared_ptr<filesystem_interface> filesystem = std::make_shared<real_filesystem>();
  return new CrudHandler(data_path_, filesystem);
}