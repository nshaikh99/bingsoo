#include "session.h"
#include "request_parser.h"
#include "request.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
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

session::session(boost::asio::io_service& io_service, std::vector<std::string> parsed_config_paths, std::string echo_path, bool is_echo, bool is_static)
  : socket_(io_service), parsed_config_paths_(parsed_config_paths), echo_path_(echo_path), 
  is_echo_(is_echo),is_static_(is_static)
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
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP unknown" << "\n";
    }

    if (parse_status == request_parser::good) {
      result = 0;

      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";

      reply::request_type req_type = get_request_type(); //echo, static, or not_found

      if (req_type == reply::type_echo){
        Echo_Request_Handler echo_request;
        reply_ = echo_request.handleRequest(data_, bytes_transferred);
      }
      else if (req_type == reply::type_static){
        //req_.uri contains the path to the file that the client is requesting
        Static_Request_Handler static_request = Static_Request_Handler(req_.uri);
        reply_ = static_request.handleRequest(data_, bytes_transferred);
      }
      else{
        reply_ = reply_.stock_reply(reply::not_found);
      }
    } 
    else if (parse_status == request_parser::bad) {
      result = 1;
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "400 Bad Request: A bad request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
      reply_ = reply_.stock_reply(reply::bad_request);
    }
    boost::asio::async_write(socket_,
        reply_.to_buffers(),
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
  if (is_echo_ && echo_path_ == req_.uri){ 
    return reply::type_echo;
  }
  else if (is_static_) { 
    for (auto original_path : parsed_config_paths_)
    {
      if (original_path == req_.uri){
        return reply::type_static;
      }
    }
  }
  return reply::type_not_found;
}