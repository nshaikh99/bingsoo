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

    // Client IP address
    try{
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with IP: " << socket_.remote_endpoint().address().to_string();
    }
    catch(...){
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with unknown IP";
    }

    string data(data_);
    request_.body() = data;
    http::response<http::string_body> response;
    RequestHandlerFactory* factory = NULL;

    cout << "\nTEST1\n";
    if (parse_status == request_parser::bad){
      RequestHandlerFactory* factory = new BadHandlerFactory(req_.uri, config_);
    }
    else{
      cout << "TEST2\n";
      RequestHandlerFactory* factory = routes_[req_.uri];
      cout << "TEST3\n";
    }
    if (factory == nullptr){
      cout << "NULL\n";
    }
    RequestHandler* handler = factory->create();
    cout << "TEST4\n";
    
    if (parse_status == request_parser::good) {
      result = 0;

      reply::request_type req_type = get_request_type(); //echo, static, or not_found

      if (req_type == reply::type_echo){
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good echo request has occurred.";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
        //factory = new EchoHandlerFactory(std::string(request_.target()), config_);
      }
      else if (req_type == reply::type_static){
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good static request has occurred.";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
        std::unordered_map<std::string,std::string> parsed_config_paths_ = config_.get_static_file_path();
        std::string parsed_static_serving_path = config_.get_static_serving_path();
        std::unordered_map<string, string>::iterator original_path;
        bool served_file = false;
        for (original_path = parsed_config_paths_.begin(); original_path != parsed_config_paths_.end(); original_path++)
        {
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Trying to Serve Static File: " << original_path->first;
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "requested uri substring is " << req_.uri;
          if (req_.uri == original_path->first){
            //factory = new StaticHandlerFactory(original_path->second, config_);
            served_file = true;
          }
          else{
            continue;
          }
        }
        if (!served_file){
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "404 NOT FOUND: A resource was requested that does not exist.";
          BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
          //factory = new _404HandlerFactory(std::string(request_.target()), config_);
        }
        //req_.uri contains the path to the file that the client is requesting
      }
      else{
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "404 NOT FOUND: A resource was requested that does not exist.";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
        //factory = new _404HandlerFactory(std::string(request_.target()), config_);
      }
    } 
    else if (parse_status == request_parser::bad) {
      result = 1;
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "400 BAD: A bad request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "\n----BEGIN REQUEST----\n" << request_info() << "----END REQUEST----";
      //factory = new BadHandlerFactory(std::string(request_.target()), config_);
    }

    handler->handle_request(request_, response);
    cout << "TEST5\n";

    std::vector<boost::asio::const_buffer> response_buffer;
    std::ostringstream response_ostring;
    response_ostring << response;
    std::string request_string = response_ostring.str();
    response_buffer.push_back(boost::asio::buffer(request_string));

    boost::asio::async_write(socket_,
        response_buffer,
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));
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
  // BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "In session.cc get_request_type().";
  if (config_.is_echo() && config_.get_echo_path() == req_.uri){ 
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Echo Request";
    return reply::type_echo;
  }
  else if (config_.is_static()) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Static Request"; 
    std::string parsed_static_serving_path = config_.get_static_serving_path();
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Requested URI is " << req_.uri;
    if (req_.uri.substr(0, parsed_static_serving_path.length()) == (parsed_static_serving_path)){
      return reply::type_static;
    }
  }
  return reply::type_not_found;
}