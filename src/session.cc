#include "session.h"
#include "request_parser.h"
#include "request.h"
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

session::session(boost::asio::io_service& io_service)
  : socket_(io_service)
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
  int res = -1;
  if (!error) 
  {
    request_parser::result_type result;
    request req;
    auto pair = req_parser_.parse(req, data_, data_ + bytes_transferred);
    result = std::get<0>(pair); //result indicates whether the parsing was done successfully 

    if (result == request_parser::good) {
      res = 0;
        reply_ = generate_response(data_, bytes_transferred, reply::ok); //loads reply with a 200 HTTP response
    } else if (result == request_parser::bad) {
      res = 1;
        reply_ = generate_response(data_, bytes_transferred, reply::bad_request); //loads reply with a 200 HTTP response
    }
    boost::asio::async_write(socket_,
        reply_.to_buffers(),
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));
    return res;
  }
  else
  {
    return res;
    delete this;
  }
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
