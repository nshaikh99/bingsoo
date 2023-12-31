#include "server.h"
#include "session.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <unordered_map>
#include <boost/thread.hpp>

using namespace std;

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port, NginxConfig config)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    config_(config)
{
  start_accept();
}

void server::run_threads()
{
  boost::thread_group thread_group;
  for (int i = 0; i < thread_count_; ++i)
     thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
  thread_group.join_all();
}

bool server::start_accept()
{
  std::unordered_map<std::string, RequestHandlerFactory*> routes = configMap();
  session* new_session = new session(io_service_, config_, routes); //initializes a tcp socket in member var socket_
  if (new_session == nullptr)
    return false;
  acceptor_.async_accept(new_session->socket(), //starts an asynchronous accept for incoming requests 
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error));
      //bind(f, x, y) returns a function object with the value f(x,y)
  return true;
}

bool server::handle_accept(session* new_session, //gets called by async_accept each time a request comes in 
    const boost::system::error_code& error)
{
  if (!error){
    new_session->start(); //session.cc: starts reading from socket stream
  }
  else{
    delete new_session;
    return false;
  }
  return (start_accept()); //once a request is fully processed, a new session is created to wait for next request
}

RequestHandlerFactory* server::createHandlerFactory(string name, string uri) {
  if (name == "StaticHandler")
    return new StaticHandlerFactory(uri, config_);
  else if (name == "EchoHandler")
    return new EchoHandlerFactory(uri, config_);
  else if (name == "BadHandler")
    return new BadHandlerFactory(uri, config_);
  else if (name == "CrudHandler")
    return new CrudHandlerFactory(uri, config_);
  else if (name == "HealthHandler")
    return new HealthHandlerFactory(uri, config_);
  else if (name == "SleepHandler")
    return new SleepHandlerFactory(uri, config_);
  else if (name == "MarkdownHandler")
    return new MarkdownHandlerFactory(uri, config_);
  else
    return new _404HandlerFactory(uri, config_);
}

std::unordered_map<std::string, RequestHandlerFactory*> server::configMap() {
  std::unordered_map<std::string, RequestHandlerFactory*> routes;
  std::unordered_map<std::string, std::string> static_path_map = config_.get_static_file_path();
  std::string echo_path = config_.get_echo_path(); 
  std::string crud_path = config_.get_crud_path();
  std::string health_path = config_.get_health_path();
  std::string sleep_path = config_.get_sleep_path();
  std::string markdown_path = config_.get_markdown_path();
  for (auto it : static_path_map){
    routes[it.first] = createHandlerFactory("StaticHandler", string(it.first));
  }
  routes[echo_path] = createHandlerFactory("EchoHandler", string(echo_path));
  routes[health_path] = createHandlerFactory("HealthHandler", string(health_path));
  routes["/"] = createHandlerFactory("_404Handler", "/");
  routes["bad"] = createHandlerFactory("BadHandler", "");
  routes[crud_path] = createHandlerFactory("CrudHandler", string(crud_path));
  routes[sleep_path] = createHandlerFactory("SleepHandler", string(sleep_path));
  routes[markdown_path] = createHandlerFactory("MarkdownHandler", string(markdown_path));
  return routes;
}