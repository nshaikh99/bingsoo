#include "gtest/gtest.h"
#include "session.h"
#include "reply.h"
#include <boost/asio.hpp>

class ReplyTestFixture : public ::testing::Test {
  protected:
    reply rep;
    boost::asio::io_service io_service;
    session* new_session = new session(io_service);
};

TEST_F(ReplyTestFixture, ValidRequest) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  rep = new_session->generate_response(input, strlen(input), reply::ok);
  bool status_success = rep.status == reply::ok ? true : false;
  
  EXPECT_TRUE(status_success);
  EXPECT_EQ(input, rep.content);
}

TEST_F(ReplyTestFixture, BadRequest) {
  char input[1024] = "fake request";
  rep = new_session->generate_response(input, strlen(input), reply::bad_request);
  bool status_failure = rep.status == reply::bad_request ? true : false;
  
  EXPECT_TRUE(status_failure);
  EXPECT_EQ(input, rep.content);
}

TEST_F(ReplyTestFixture, StockReply) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  rep = new_session->generate_response(input, strlen(input), reply::ok);
  bool status_success = rep.status == reply::ok ? true : false;
  
  rep.stock_reply(reply::ok);
  EXPECT_TRUE(status_success);
}

TEST_F(ReplyTestFixture, ToStringReply) {
  char input[1024] = "GET / HTTP/1.1\r\nHost: www.test.com\r\nConnection: close\r\n\r\n";
  rep = new_session->generate_response(input, strlen(input), reply::ok);
  bool status_success = rep.status == reply::ok ? true : false;
  
  // these lines make sure every type of status for a reply is accepted
  rep.stock_reply(reply::ok);
  rep.stock_reply(reply::created);
  rep.stock_reply(reply::accepted);
  rep.stock_reply(reply::no_content);
  rep.stock_reply(reply::multiple_choices);
  rep.stock_reply(reply::moved_permanently);
  rep.stock_reply(reply::moved_temporarily);
  rep.stock_reply(reply::not_modified);
  rep.stock_reply(reply::bad_request);
  rep.stock_reply(reply::unauthorized);
  rep.stock_reply(reply::forbidden);
  rep.stock_reply(reply::not_found);
  rep.stock_reply(reply::internal_server_error);
  rep.stock_reply(reply::not_implemented);
  rep.stock_reply(reply::bad_gateway);
  rep.stock_reply(reply::service_unavailable);


  EXPECT_TRUE(status_success);
}

