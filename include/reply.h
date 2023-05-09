//
// reply.h
// ~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef reply_h
#define reply_h

#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "header.h"

// A reply to be sent to a client.
struct reply
{
  // The status of the reply.
  enum status_type
  {
    ok = 200,
    bad_request = 400,
    not_found = 404,
    internal_server_error = 500,
  } status;

<<<<<<< HEAD
  enum request_type {type_echo, type_static, type_not_found};

  /// The headers to be included in the reply.
=======
  // The headers to be included in the reply.
>>>>>>> 187db93 (Code cleanup and additional logging)
  std::vector<header> headers;

  // The content to be sent in the reply.
  std::string content;

  // Convert the reply into a vector of buffers. The buffers do not own the
  // underlying memory blocks, therefore the reply object must remain valid and
  // not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();

  // Get a stock reply.
  static reply stock_reply(status_type status);
};

#endif
