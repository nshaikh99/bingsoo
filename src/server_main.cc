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

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "global.h"

using boost::asio::ip::tcp;

void handler(const boost::system::error_code& error, int signum)
{
  BOOST_LOG_TRIVIAL(fatal) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::FATAL] << "Terminated program with signal: " << signum;

  exit(signum); // Terminate program
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: bin/server <config>\n";
      return 1;
    }

    // logging setup

    boost::log::add_common_attributes();

    boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%"
    );

    boost::log::add_file_log(
      boost::log::keywords::file_name = "./logs/log_%N.log",
      boost::log::keywords::rotation_size = 1024 * 1024 * 10, // 1024 bytes/kilobyte * 1024 kilobytes/megabyte * 10 megabytes
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // hour 0, minute 0, second 0
      boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%"
    );

    // config file parsing

    NginxConfigParser config_parser;
    NginxConfig config;
    bool parsed_config = config_parser.Parse(argv[1], &config);
    if (!parsed_config){
      BOOST_LOG_TRIVIAL(fatal) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::FATAL] << "Invalid config file";
      return -1;
    }
    BOOST_LOG_TRIVIAL(trace) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::TRACE] << "Successfully parsed config file";
    // BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << parsed_config;


    // extract info from config file

    int port_num = config.get_port_num();
    if (port_num == -1) {
      BOOST_LOG_TRIVIAL(fatal) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::FATAL] << "Invalid port number in config file";
      return -1;
    }

    // instantiate server

    boost::asio::io_service io_service;

    server s(io_service, port_num, config); //calls the server::server(...) function in server.cc
    BOOST_LOG_TRIVIAL(trace) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::TRACE] << "Server running on port: " << port_num;

    // handle signals
    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    signals.async_wait(handler);

    // run server
    s.run_threads();

    // Sample error messages
    // BOOST_LOG_TRIVIAL(trace) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::TRACE] << "A trace severity message";
    // BOOST_LOG_TRIVIAL(debug) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::DEBUG] << "A debug severity message";
    // BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "An informational severity message";
    // BOOST_LOG_TRIVIAL(warning) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::WARNING] << "A warning severity message";
    // BOOST_LOG_TRIVIAL(error) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::ERROR] << "An error severity message";
    // BOOST_LOG_TRIVIAL(fatal) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::FATAL] << "A fatal severity message";
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

