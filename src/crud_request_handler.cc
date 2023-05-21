#include "crud_request_handler.h"

CrudHandler::CrudHandler(std::string path){
    data_path_ = path;
}

status CrudHandler::handle_request( const http::request<http::string_body> req, http::response<http::string_body> &res) {
    return false;
}
