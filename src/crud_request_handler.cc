#include "crud_request_handler.h"

CrudHandler::CrudHandler(std::string path): data_path_(path){}

status CrudHandler::handle_request( const http::request<http::string_body> req, http::response<http::string_body> &res) {
    if (req.method() == http::verb::post) {
      // POST request

      // Find next available ID 
      // Create new instance of Entity ()
      //  if Entity doesn't exist, create it first? (i.e. make new directory)
      // Write POST body contents (JSON) to newly created instance
      // Return ID of newly created instance
      return false;
    }


    
    return false;
}
