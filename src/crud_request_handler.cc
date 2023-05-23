#include <filesystem>
#include <vector>
#include <algorithm>

// TEST
#include <iostream>

#include "crud_request_handler.h"

CrudHandler::CrudHandler(std::string path, std::shared_ptr<filesystem_interface> filesystem): 
    data_path_(path), filesystem_(filesystem){}

status CrudHandler::handle_request( const http::request<http::string_body> req, http::response<http::string_body> &res) {
    if (req.method() == http::verb::post) {
      // POST request
      std::string target = req.target().to_string();
      // make sure ID is not passed in POST request.
      std::string basename = target.substr(target.find_last_of("/") + 1);
      bool isID = true;
      for (int i = 0; i < basename.size(); i++) {
        if (!isdigit(basename[i])) {
          isID = false;
          break;
        }
      }
      if (isID) {
        // TO-DO: Return 404 Not Found
      }

      // Create new instance of Entity ()
      // get /api/{THIS PART}
      std::string suffix;
      size_t firstSlashPos = target.find('/');
      if (firstSlashPos != std::string::npos) {
        size_t secondSlashPos = target.find('/', firstSlashPos + 1);
        if (secondSlashPos != std::string::npos) {
            suffix = target.substr(secondSlashPos + 1);
        }
      }

      std::string entityPath = "./" + data_path_ + "/" + suffix;

      std::string instancePath;
      int ID = 1; // Start w/ default ID
      if (!filesystem_->file_exists(entityPath)) {
        // Entity doesn't exist. Create and make instance with ID=1
        instancePath = entityPath + "/1";
      } else {
        //  Find next available ID for existing Entity
        while ( filesystem_->file_exists(entityPath + "/" + std::to_string(ID) ) ){
          ID += 1;
        }
        instancePath = entityPath + "/" + std::to_string(ID);
      }
      // Write POST body contents (JSON) to newly created instance
      std::string POST_data = req.body().data();
      filesystem_->write_file(instancePath, POST_data);

      // Return ID of newly created instance in response
      res.body() = "{\"id\": " + std::to_string(ID) + "}";
      res.result(http::status::created); 
      res.content_length((res.body().size()));
      res.set(http::field::content_type, "application/json");
      return true;
    }


    
    return false;
}
