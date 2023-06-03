#include "markdown_handler_factory.h"
#include "markdown_handler.h"

MarkdownHandlerFactory::MarkdownHandlerFactory(const std::string& path, const NginxConfig& config)
: path_(path), config_(config) {}

RequestHandler* MarkdownHandlerFactory::create()
{
  return new MarkdownHandler();
}