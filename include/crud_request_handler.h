#ifndef crud_request_handler_h
#define crud_request_handler_h

#include <string>
#include <boost/beast/http.hpp>
#include "request_handler.h"
#include "filesystem_interface.h"

namespace http = boost::beast::http;
//namespace fs = boost::filesystem;
typedef bool status;

class CrudHandler : public RequestHandler {
  public:
    CrudHandler(std::string path, std::shared_ptr<filesystem_interface> filesystem);
    status handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res);
    std::string string_vector_to_json(std::vector<std::string> elementList);
  private:
    std::string data_path_;
    std::shared_ptr<filesystem_interface> filesystem_;
};

#endif // crud_request_handler_h