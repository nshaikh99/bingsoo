#include "session.h"
#include "request_parser.h"
#include "request.h"
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "global.h"

using boost::asio::ip::tcp;
using namespace std;

session::session(boost::asio::io_service& io_service, NginxConfig config, std::unordered_map<std::string, RequestHandlerFactory*> routes)
  : socket_(io_service), config_(config), routes_(routes)
{
}

tcp::socket& session::socket()
{
  return socket_;
}

bool session::start()
{
  socket_.async_read_some(boost::asio::buffer(data_, max_length), //places data from socket stream into buffer
      boost::bind(&session::handle_read, this,                    //and passess it to handle_read
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
  return true;
}

//reads data from buffer and passes it to handle_write
int session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{

  // BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "In session.cc handle_read().";
  int result = -1; //if result is still -1 this means an error has occured
  if (!error) {
    request_parser::result_type parse_status;
    auto pair = req_parser_.parse(req_, data_, data_ + bytes_transferred);
    parse_status = std::get<0>(pair); //parse_status indicates whether the parsing was done successfully 
    string IP_address = "";

    // Client IP address
    try{
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with IP: " << socket_.remote_endpoint().address().to_string();
      IP_address = socket_.remote_endpoint().address().to_string();
    }
    catch(...){
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with unknown IP";
    }

    string data(data_);
    request_.method_string(req_.method);
    request_.body() = data;
    request_.target(req_.uri);
    // http::response<http::string_body> response;
    RequestHandlerFactory* factory = NULL;

    if (parse_status == request_parser::bad){
      //factory = new BadHandlerFactory(req_.uri, config_);
      factory = routes_["bad"];
    }
    else{
      factory = routes_[req_.uri];
    }
    if (factory == nullptr){
      factory = routes_["/"]; // "/" is mapped to the 404 handler factory
    }
    
    string handler_type = "";

    if (parse_status == request_parser::good) {
      result = 0;

      reply::request_type req_type = get_request_type(); //echo, static, crud, health, sleep, markdown, or not_found

      if (req_type == reply::type_echo){
        handler_type = "Echo Handler";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good echo request has occurred.";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
      }
      else if (req_type == reply::type_static){
        handler_type = "Static Handler";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good static request has occurred.";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
        std::unordered_map<std::string,std::string> parsed_config_paths_ = config_.get_static_file_path();
        std::string parsed_static_serving_path = config_.get_static_serving_path();
        std::unordered_map<string, string>::iterator original_path;
        bool served_file = false;
        for (original_path = parsed_config_paths_.begin(); original_path != parsed_config_paths_.end(); original_path++)
        {
          if (req_.uri == original_path->first){
            factory = new StaticHandlerFactory(original_path->second, config_);
            served_file = true;
          }
          else{
            continue;
          }
        }
        if (!served_file){
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "404 NOT FOUND: A resource was requested that does not exist.";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
          factory = routes_["/"];
        }
      } else if (req_type == reply::type_crud){
          handler_type = "CRUD Handler";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good CRUD request has occurred.";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
          std::string parsed_crud_path = config_.get_crud_path();
          factory = routes_[parsed_crud_path];
      } else if (req_type == reply::type_health){
          handler_type = "Health Handler";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good health request has occurred.";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
      } else if (req_type == reply::type_sleep){
        handler_type = "Sleep Handler";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good sleep request has occurred.";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
      } else if (req_type == reply::type_markdown){
        handler_type = "Markdown Handler";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good markdown request has occurred.";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
      }
      else{
        handler_type = "404 Handler";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "404 NOT FOUND: A resource was requested that does not exist.";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]"  << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
      }
    } 
    else if (parse_status == request_parser::bad) {
      handler_type = "Bad Request Handler";
      result = 1;
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "400 BAD: A bad request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[RequestMetrics]" << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
    }
    RequestHandler* handler = factory->create();
    handler->handle_request(request_, response_);


    std::vector<boost::asio::const_buffer> response_buffer;
    std::ostringstream response_ostring;
    response_ostring << response_;
    std::string request_string = response_ostring.str();
    response_buffer.push_back(boost::asio::buffer(request_string));

    boost::asio::async_write(socket_,
        response_buffer,
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));
    
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[ResponseMetrics] Response Code: " << response_.result_int() << ", Request Path: " << req_.uri << ", Request IP: " << IP_address << ", Request Handler: " << handler_type;

  }
  else
  {
    delete this;
  }
  return result;
}

//writes max amount of data to stream. if there's more than max data, the rest gets sent back to handle_read
void session::handle_write(const boost::system::error_code& error)
{
  // BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "In session.cc handle_write().";
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length), 
        boost::bind(&session::handle_read, this,                    
          boost::asio::placeholders::error,                         
          boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    delete this;
  }
}

std::string session::request_info()
{
  std::string request = "Method: " + req_.method + "\n" +
                        "Uri: " + req_.uri + "\n" +
                        "HTTP Version: " + std::to_string(req_.http_version_major) + "." +
                        std::to_string(req_.http_version_minor) + "\n";
  for (int i = 0; i < req_.headers.size(); i++) 
  {
    request += "Header " + std::to_string(i) + ": " + "\n" +
          "Name: " + req_.headers.at(i).name + "\n" +
          "Value: " + req_.headers.at(i).value + "\n";
  }
  
  return request;
}

reply::request_type session::get_request_type()
{
  BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Requested URI is: " << req_.uri;
  string echo_path = config_.get_echo_path();
  string static_path = config_.get_static_serving_path();
  string crud_path = config_.get_crud_path();
  string health_path = config_.get_health_path();
  string sleep_path = config_.get_sleep_path();
  string markdown_path = config_.get_markdown_path();
  if (echo_path != "" && req_.uri.rfind(echo_path, 0) == 0){
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Echo Request";
    return reply::type_echo;
  }
  else if (static_path != "" && (req_.uri.rfind(static_path, 0) == 0)) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Static Request"; 
    return reply::type_static;
  } 
  else if (crud_path != "" && (req_.uri.rfind(crud_path, 0) == 0)) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "CRUD Request";
    return reply::type_crud;
  }
  else if (health_path != "" && (req_.uri.rfind(health_path, 0) == 0)) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Health Request";
    return reply::type_health;
  }
  else if (sleep_path != "" && (req_.uri.rfind(sleep_path, 0) == 0)) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Sleep Request";
    return reply::type_sleep;
  }
  else if (markdown_path != "" && (req_.uri.rfind(markdown_path, 0) == 0)) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Markdown Request";
    return reply::type_markdown;
  }
  return reply::type_not_found;
}