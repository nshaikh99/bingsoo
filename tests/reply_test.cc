#include "gtest/gtest.h"
#include "session.h"
#include "reply.h"
#include <boost/asio.hpp>

using namespace std;

class ReplyTestFixture : public ::testing::Test {
  protected:
    reply rep;
}; 

TEST_F(ReplyTestFixture, OkRequest_Stock){
  rep = reply::stock_reply(reply::ok);
  rep.to_buffers();
  bool status_success = rep.status == reply::ok;
  bool content_success = rep.content == stock_replies::ok;
  bool content_size_success = rep.headers[0].value == to_string(rep.content.size());
  bool content_type_success = rep.headers[1].value == "text/html";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
}

TEST_F(ReplyTestFixture, NotFoundRequest_Stock){
  rep = reply::stock_reply(reply::not_found);
  bool status_success = rep.status == reply::not_found;
  bool content_success = rep.content == stock_replies::not_found;
  bool content_size_success = rep.headers[0].value == to_string(rep.content.size());
  bool content_type_success = rep.headers[1].value == "text/html";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
}

TEST_F(ReplyTestFixture, BadRequest_Stock){
  rep = reply::stock_reply(reply::bad_request);
  bool status_success = rep.status == reply::bad_request;
  bool content_success = rep.content == stock_replies::bad_request;
  bool content_size_success = rep.headers[0].value == to_string(rep.content.size());
  bool content_type_success = rep.headers[1].value == "text/html";
  EXPECT_TRUE(status_success && content_success && content_size_success && content_type_success);
}