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
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

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

    boost::log::add_common_attributes();

    boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%"
    );

    boost::log::add_file_log(
      boost::log::keywords::file_name = "../logs/log_%N.log",
      boost::log::keywords::rotation_size = 1024 * 1024 * 10,
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%"
    );

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

    boost::asio::io_service io_service;

    server s(io_service, port_num); //calls the server::server(...) function in server.cc

    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
