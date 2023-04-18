//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

using namespace std; // For atoi.

#include "session.h"
#include "server.h"
#include "config_parser.h"

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: bin/server <config>\n";
      return 1;
    }

    NginxConfigParser config_parser;
    NginxConfig config;
    bool parsed_config = config_parser.Parse(argv[1], &config);
    if (!parsed_config){
      cerr << "Invalid config file" << std::endl;
      return -1;
    }

    int port_num = config.get_port_num();
    if (port_num == -1) {
      cerr << "Invalid port number in config file" << std::endl;
      return -1;
    }
    // cout<<"the port num is "<<port_num<<endl;

    boost::asio::io_service io_service;

    server s(io_service, port_num); //calls the server::server(...) function in server.cc

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
