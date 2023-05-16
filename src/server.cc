#include "server.h"
#include "session.h"

#include "request_handler_factory.h"
#include "404_handler_factory.h"
#include "echo_handler_factory.h"
#include "static_handler_factory.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <unordered_map>

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
  //std::unordered_map<std::string, RequestHandlerFactory*> routes = configMap();
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

// RequestHandlerFactory* createHandlerFactory(const string& name, string uri) {
//   if (name == "StaticHandler")
//     return new StaticHandlerFactory(uri, config_);
//   else if (name == "EchoHandler")
//     return new EchoHandlerFactory(uri, config_);
//   else
//     return new _404HandlerFactory(uri, config_);
// }

// std::unordered_map<std::string, RequestHandlerFactory*> configMap() {
//   std::unordered_map<std::string, RequestHandlerFactory*> routes;
//   std::unordered_map<std::string, std::string> static_path_map = config_.get_static_file_path();
//   std::string echo_path = config_.get_echo_path(); 
//   for (auto it : static_path_map){
//     routes[it.first] = createHandlerFactory( "StaticHandler", it.first);
//   }
//   routes[echo_path] = createHandlerFactory( "EchoHandler", echo_path);
//   routes["/"] = createHandlerFactory( "_404Handler", "/");
//   return routes;
// }

