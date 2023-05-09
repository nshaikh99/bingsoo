#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "session.h"
#include "request_parser.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"

#include "global.h"

using boost::asio::ip::tcp;
using namespace std;

<<<<<<< HEAD
session::session(boost::asio::io_service& io_service, NginxConfig config)
  : socket_(io_service), config_(config)
{
}
=======
session::session(boost::asio::io_service& io_service, std::vector<std::string> parsed_config_paths, std::string echo_path, bool is_echo, bool is_static)
  : socket_(io_service),
    parsed_config_paths_(parsed_config_paths),
    echo_path_(echo_path), 
    is_echo_(is_echo),
    is_static_(is_static) {}
>>>>>>> 187db93 (Code cleanup and additional logging)

tcp::socket& session::socket()
{
  return socket_;
}

<<<<<<< HEAD
=======
reply session::generate_response(char *data_, int bytes_transferred, reply::status_type status)
{
  reply reply_;
  reply_.status = status;
  reply_.content = string(data_, bytes_transferred);
  header length_header = {"Content-Length", to_string(bytes_transferred)};
  header type_header = {"Content-Type", "text/plain"};
  reply_.headers = {length_header, type_header};
  return reply_;
}

>>>>>>> 187db93 (Code cleanup and additional logging)
bool session::start()
{
  socket_.async_read_some(
    boost::asio::buffer(data_, max_length), // places data from socket stream into buffer
    boost::bind(&session::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) // and passes it to handle_read
  );
  return true;
}

// reads data from buffer and passes it to handle_write
int session::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
<<<<<<< HEAD
  BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "In session.cc handle_read().";
  int result = -1; //if result is still -1 this means an error has occured
  if (!error) {
    request_parser::result_type parse_status;
    auto pair = req_parser_.parse(req_, data_, data_ + bytes_transferred);
    parse_status = std::get<0>(pair); //parse_status indicates whether the parsing was done successfully 

    // Client IP address
    try{
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP: " << socket_.remote_endpoint().address().to_string() << "\n";
    }
    catch(...){
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP unknown.";
    }

    if (parse_status == request_parser::good) {
      result = 0;

      reply::request_type req_type = get_request_type(); //echo, static, or not_found

      if (req_type == reply::type_echo){
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good echo request has occurred.\n";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
        Echo_Request_Handler echo_request;
        reply_ = echo_request.handleRequest(data_, bytes_transferred);
      }
      else if (req_type == reply::type_static){
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good static request has occurred.\n";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
        //req_.uri contains the path to the file that the client is requesting
        Static_Request_Handler static_request = Static_Request_Handler(req_.uri);
        reply_ = static_request.handleRequest(data_, bytes_transferred);
      }
      else{
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "404 NOT FOUND: A resource was requested that does not exist.\n";
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
        reply_ = reply_.stock_reply(reply::not_found);
      }
    } 
    else if (parse_status == request_parser::bad) {
      result = 1;
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "400 BAD REQUEST: A bad request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
      reply_ = reply_.stock_reply(reply::bad_request);
=======
  int result = -1; // if result is still -1 then this means that an error has occured
  if(!error) 
  {
    request_parser::result_type parse_status;
    auto pair = req_parser_.parse(req, data_, data_ + bytes_transferred);
    parse_status = std::get<0>(pair); // indicates whether the parsing was done successfully 
    bool is_echo_request = is_echo_;
    bool is_static_request = is_static_;
    if(parse_status == request_parser::good)
    {
      bool echoed = false;
      result = 0;
      // log client IP
      try
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with IP: " << socket_.remote_endpoint().address().to_string();
      }
      catch(...)
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with unknown IP";
      }
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good request has occurred.";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "----BEGIN REQUEST----\n" << request_info() << "\n----END REQUEST----";
      if(is_echo_request)
      {
        // echo reply case
        if (echo_path_ == req.uri)
        {
          Echo_Request_Handler echo_request;
          reply_ = echo_request.handleRequest(data_, bytes_transferred, reply::ok);
          echoed = true;
        }
      }
      if(is_static_request)
      {
        // static reply case
        bool served = false;
        for(auto original_path : parsed_config_paths_)
        {
          if(original_path == req.uri)
          {
            std::string path_with_dot = "."+original_path;
            Static_Request_Handler static_request = Static_Request_Handler(path_with_dot);
            reply_ = static_request.handleRequest(data_, bytes_transferred, reply::ok);
            served = true;
          }
        }
        if(!served && !echoed)
        {
          reply_ = reply_.stock_reply(reply::not_found);
        }
      }
      if(!is_static_request && !is_echo_request)
      {
        reply_ = reply_.stock_reply(reply::not_found);
      }
    }
    else if(parse_status == request_parser::bad)
    {
      result = 1;
      // log client IP
      try
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with IP: " << socket_.remote_endpoint().address().to_string() << std::endl;
      }
      catch(...)
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Incoming request from client with unknown IP" << std::endl;
      }
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "400 BAD: A bad request has occurred.";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "----BEGIN REQUEST----\n" << request_info() << "\n----END REQUEST----";
      reply_ = generate_response(data_, bytes_transferred, reply::bad_request);
>>>>>>> 187db93 (Code cleanup and additional logging)
    }
    boost::asio::async_write(
      socket_,
      reply_.to_buffers(),
      boost::bind(&session::handle_write, this,
      boost::asio::placeholders::error)
    );
  }
  else
  {
    delete this;
  }
  return result;
}

// writes max amount of data to stream; if there is more than max data, then the rest gets sent back to handle_read
void session::handle_write(const boost::system::error_code& error)
{
<<<<<<< HEAD
  BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "In session.cc handle_write().";
  if (!error)
=======
  if(!error)
>>>>>>> 187db93 (Code cleanup and additional logging)
  {
    socket_.async_read_some(
      boost::asio::buffer(data_, max_length), 
      boost::bind(&session::handle_read, this,                    
      boost::asio::placeholders::error,                         
      boost::asio::placeholders::bytes_transferred)
    );
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
  BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "In session.cc get_request_type().";
  if (config_.is_echo() && config_.get_echo_path() == req_.uri){ 
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Echo Request";
    return reply::type_echo;
  }
  else if (config_.is_static()) {
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Static Request"; 
    std::vector<std::string> parsed_config_paths_ = config_.get_static_file_path();
    for (auto original_path : parsed_config_paths_)
    {
      if (original_path == req_.uri){
        return reply::type_static;
      }
    }
  }
  return reply::type_not_found;
}