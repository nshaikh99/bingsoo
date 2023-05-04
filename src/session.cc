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

reply session::generate_response(char *data_, int bytes_transferred, reply::status_type status) {
  using namespace std;
  reply reply_;
  reply_.status = status;
  reply_.content = string(data_, bytes_transferred);
  header length_header = {"Content-Length", to_string(bytes_transferred)};
  header type_header = {"Content-Type", "text/plain"};
  reply_.headers = {length_header, type_header};
  return reply_;
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
  if (!error) 
  {
    request_parser::result_type parse_status;
    auto pair = req_parser_.parse(req, data_, data_ + bytes_transferred);
    parse_status = std::get<0>(pair); //parse_status indicates whether the parsing was done successfully 

    bool is_echo_request = is_echo_;
    bool is_static_request = is_static_;
    if (parse_status == request_parser::good) {
      bool echoed = false;
      result = 0;

      // Client IP address
      try
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP: " << socket_.remote_endpoint().address().to_string() << std::endl;
      }
      catch(...)
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP unknown" << std::endl;
      }
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "200 OK: A good request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
      if (is_echo_request){
        // echo reply case
        if (echo_path_ == req.uri)
        {
          Echo_Request_Handler echo_request;
          reply_ = echo_request.handleRequest(data_, bytes_transferred, reply::ok);
          echoed = true;
        }
      }
      if (is_static_request) {
        // static reply case
        bool served = false;
        for (auto original_path : parsed_config_paths_)
        {
          if (original_path == req.uri)
          {
            std::string path_with_dot = "."+original_path;
            Static_Request_Handler static_request = Static_Request_Handler(path_with_dot);
            reply_ = static_request.handleRequest(data_, bytes_transferred, reply::ok);
            served = true;
          }
        }
        if (!served && !echoed)
        {
          reply_ = reply_.stock_reply(reply::not_found);
        }
      }
      if (!is_static_request && !is_echo_request) {
        reply_ = reply_.stock_reply(reply::not_found);
      }
    } else if (parse_status == request_parser::bad) {
      result = 1;
      try
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP: " << socket_.remote_endpoint().address().to_string() << std::endl;
      }
      catch(...)
      {
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Client IP unknown" << std::endl;
      }
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "400 Bad Request: A bad request has occurred.\n";
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Request:\n" << request_info() << "\n";
      reply_ = generate_response(data_, bytes_transferred, reply::bad_request);
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
  std::string request = "Method: " + req.method + "\n" +
                        "Uri: " + req.uri + "\n" +
                        "HTTP Version: " + std::to_string(req.http_version_major) + "." +
                        std::to_string(req.http_version_minor) + "\n";
  for (int i = 0; i < req.headers.size(); i++) 
  {
    request += "Header " + std::to_string(i) + ": " + "\n" +
          "Name: " + req.headers.at(i).name + "\n" +
          "Value: " + req.headers.at(i).value + "\n";
  }
  
  return request;
}
