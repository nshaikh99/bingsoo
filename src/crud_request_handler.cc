#include <filesystem>
#include <vector>
#include <algorithm>

// TEST
#include <iostream>

#include "crud_request_handler.h"
#include "global.h"

CrudHandler::CrudHandler(std::string path, std::shared_ptr<filesystem_interface> filesystem): 
    data_path_(path), filesystem_(filesystem){}

status CrudHandler::handle_request( const http::request<http::string_body> req, http::response<http::string_body> &res) {

  std::string target = req.target().to_string();

  // get /api/{THIS PART}
  std::string suffix;
  size_t firstSlashPos = target.find('/');
  if (firstSlashPos != std::string::npos) {
    size_t secondSlashPos = target.find('/', firstSlashPos + 1);
    if (secondSlashPos != std::string::npos) {
        suffix = target.substr(secondSlashPos + 1);
    }
    else{
      res.body() = "Request uri " + target + " only has 1 slash, instead of 2\n";
      res.result(http::status::bad_request);
      res.content_length((res.body().size()));
      res.set(http::field::content_type, "application/json");
      return false;
    }
  }
  else{
    res.body() = "Request uri " + target + " has no slashes\n";
    res.result(http::status::bad_request);
    res.content_length((res.body().size()));
    res.set(http::field::content_type, "application/json");
    return false;
  }

  
  std::string path_to_entities = "." + data_path_ + "/" + suffix;
  std::cout << "\nCRUD PATH: \n" << path_to_entities << "\nEND CRUD PATH\n";

  std::string basename = target.substr(target.find_last_of("/") + 1);
  bool isID = true;
  for (int i = 0; i < basename.size(); i++) {
    if (!isdigit(basename[i])) {
      isID = false;
      break;
    }
  }

  if (req.method() == http::verb::post) {
    // POST request
    // make sure ID is not passed in POST request.
    
    if (isID) {
      // If there is an id, return 404 Not Found
      res.body() = "id in request uri of post request\n";
      res.result(http::status::bad_request);
      res.content_length((res.body().size()));
      res.set(http::field::content_type, "application/json");
      return false;
    }

    // If there is no id, create new instance of Entity ()
    

    std::string instancePath;
    int ID = 1; // Start w/ default ID
    if (!filesystem_->file_exists(path_to_entities)) {
      // Entity doesn't exist. Create and make instance with ID=1
      instancePath = path_to_entities + "/1";
    } else {
      //  Find next available ID for existing Entity
      while ( filesystem_->file_exists(path_to_entities + "/" + std::to_string(ID) ) ){
        ID += 1;
      }
      instancePath = path_to_entities + "/" + std::to_string(ID);
    }
    // Write POST body contents (JSON) to newly created instance
    std::string body_data = req.body().data();
    std::string POST_data = body_data.substr(body_data.find("\r\n\r\n")+4); //skip the headers
    filesystem_->write_file(instancePath, POST_data);

    // Return ID of newly created instance in response
    res.body() = "{\"id\": " + std::to_string(ID) + "}\n";
    res.result(http::status::created); 
    res.content_length((res.body().size()));
    res.set(http::field::content_type, "application/json");
    return true;
  }


  else if (req.method() == http::verb::get) {
    // GET request

    //get all the ids if a resource is requested without an id

    //find the last occurrence of /
    std::string endpoint = path_to_entities.substr(path_to_entities.find_last_of("/") + 1);
    try{
      //check if the remaining url can be converted into a number or not
      int end_num = std::stoi(endpoint);

    }
    catch(...){
      //if not, we want to fetch a list of the relevant ids for that resource
      
      //if the file exists, get the ids and return them
      if(filesystem_->file_exists(path_to_entities)){
        std::vector<std::string> allIds = filesystem_->list_files(path_to_entities);

        //convert vector to json array
        std::string idList = string_vector_to_json(allIds);

        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = idList + "\n";

        return true;
      }
      else{
        //if the directory does not exist, return a 404 not found response
        res.result(http::status::not_found);
        res.set(http::field::content_type, "application/json");
        res.body() = "Directory not found\n";
        res.content_length((res.body().size()));

        return false;

      }

    }
    //if an id exists, then we want to access a specific resource

    //if the file exists, get the specific data
    if(filesystem_->file_exists(path_to_entities)){
      std::string file_content;
      bool result = filesystem_->read_file(path_to_entities, file_content);

      //if we have successfully read it, return the contents
      if(result) {
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = file_content;
        res.content_length((res.body().size()));

        return true;
      }
      //else, log an error and return an erroneous response
      else{
        // BOOST_LOG_TRIVIAL(error) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::ERROR] << "Failed to read the contents of file: " << path_to_entities;

        res.result(http::status::bad_request);
        res.content_length((res.body().size()));
        res.set(http::field::content_type, "application/json");
        res.body() = "Error reading file\n";

        return false;
      }
    }
    else{
      res.result(http::status::not_found);
      res.set(http::field::content_type, "application/json");
      res.body() = "File does not exist\n";
      res.content_length((res.body().size()));

      return false; 
    }

  }

  if (req.method() == http::verb::delete_) {
    // DELETE request
    //check if there is an id or not
    if(isID){
      //if there is one 
      //check if the file exists
      if(filesystem_->file_exists(path_to_entities)){
        //if the file exists, delete it
        bool result = filesystem_->delete_file(path_to_entities);
        if(result){
          //if the file was successfully deleted, return a response
          res.result(http::status::ok);
          res.set(http::field::content_type, "application/json");
          res.body() = "Deleted " + path_to_entities + " from the server\n";;
          res.content_length((res.body().size()));
          return true;
        }
        else{
          //if there was an error during deletion, return an erroneous response
          res.result(http::status::bad_request);
          res.content_length((res.body().size()));
          res.set(http::field::content_type, "application/json");
          res.body() = "Error deleting file\n";
          return false; 
        }
      }
      else{
        //if the file does not exist, return error response
        res.result(http::status::not_found);
        res.content_length((res.body().size()));
        res.set(http::field::content_type, "application/json");
        res.body() = "File does not exist\n";
        return false; 
      }
    }
    else{
      //if there isn't one, return bad request
      // Return 404 Not Found
      res.body() = "numerical id not in request uri of delete request\n";
      res.result(http::status::bad_request);
      res.content_length((res.body().size()));
      res.set(http::field::content_type, "application/json");
      return false;
    }
  }
  if (req.method() == http::verb::put) {
    // PUT request
    //If id exists
    if(isID){
      //find file
      //edit file
      // Write PUT body contents (JSON) to newly created instance
      std::string body_data = req.body().data();
      std::string PUT_data = body_data.substr(body_data.find("\r\n\r\n")+4); //skip the headers
      if(filesystem_->file_exists(path_to_entities)){
        res.result(http::status::ok);
    
      }
      else{
        res.result(http::status::created);
      }
      res.set(http::field::content_type, "application/json");
      res.body() = "Edited " + path_to_entities + " on the server\n";
      filesystem_->write_file(path_to_entities, PUT_data);
      return true;
    }
    else{
      //If id does not exist
      //bad request
      res.body() = "numerical id not in request uri of put request\n";
      res.result(http::status::bad_request);
      res.content_length((res.body().size()));
      res.set(http::field::content_type, "application/json");
      return false;
    }
      
  }
  
  return false;
}

std::string CrudHandler::string_vector_to_json(std::vector<std::string> elementList) {
    std::string jsonResult = "[";
    for (int i = 0; i < elementList.size(); i++) {
        jsonResult += elementList[i];
        if (i != elementList.size() - 1) {
            jsonResult += ", "; // add comma if not last element
        }
    }
    jsonResult += "]";
    return jsonResult;
}