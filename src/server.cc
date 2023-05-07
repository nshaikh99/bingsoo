#include "server.h"
#include "session.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port, NginxConfig config)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    config_(config)
{
  start_accept();
}

void server::start_accept()
{
  session* new_session = new session(io_service_, config_); //initializes a tcp socket in member var socket_
  acceptor_.async_accept(new_session->socket(), //starts an asynchronous accept for incoming requests 
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));
      //bind(f, x, y) returns a function object with the value f(x,y)
}

void server::handle_accept(session* new_session, //gets called by async_accept each time a request comes in 
    const boost::system::error_code& error)
{
  if (!error)
  {
    new_session->start(); //session.cc: starts reading from socket stream
  }
  else
  {
    delete new_session;
  }

  start_accept(); //once a request is fully processed, a new session is created to wait for next request
}
