#include "markdown_handler_factory.h"
#include "markdown_handler.h"

MarkdownHandlerFactory::MarkdownHandlerFactory(const std::string& location_path_, const NginxConfig& config)
: location_path_(location_path_), config_(config) {
  std::unordered_map<std::string,std::string> args_map = config_.get_markdown_args();
  data_path_ = args_map["data_path"];
}

RequestHandler* MarkdownHandlerFactory::create()
{
  return new MarkdownHandler(data_path_);
}