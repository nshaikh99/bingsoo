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
  bool success1 = rep.status == reply::ok;
  bool success2 = rep.content == stock_replies::ok;
  bool success3 = rep.headers[0].value == to_string(rep.content.size());
  bool success4 = rep.headers[1].value == "text/html";
  EXPECT_TRUE(success1 && success2 && success3 && success4);
}

TEST_F(ReplyTestFixture, NotFoundRequest_Stock){
  rep = reply::stock_reply(reply::not_found);
  bool success1 = rep.status == reply::not_found;
  bool success2 = rep.content == stock_replies::not_found;
  bool success3 = rep.headers[0].value == to_string(rep.content.size());
  bool success4 = rep.headers[1].value == "text/html";
  EXPECT_TRUE(success1 && success2 && success3 && success4);
}

TEST_F(ReplyTestFixture, BadRequest_Stock){
  rep = reply::stock_reply(reply::bad_request);
  bool success1 = rep.status == reply::bad_request;
  bool success2 = rep.content == stock_replies::bad_request;
  bool success3 = rep.headers[0].value == to_string(rep.content.size());
  bool success4 = rep.headers[1].value == "text/html";
  EXPECT_TRUE(success1 && success2 && success3 && success4);
}

//TODO: Finish this buggy test:

// string convert_buffer(vector<boost::asio::const_buffer> buffer){
//   string res_response = "";
//   for (int i = 0; i < buffer.size(); i++){
//     const unsigned char* unsigned_char = static_cast<const unsigned char*> (buffer.at(i).data());
//     const char* signed_char = (char*) unsigned_char;
//     string final_char = std::string{signed_char};
//     res_response += final_char;
//   }
//   return res_response;
// }

// TEST_F(ReplyTestFixture, ToBuffers){
//   vector<boost::asio::const_buffer> buffer;
//   string request = "sample request";
//   string response_protocol = "HTTP/1.0 200 OK\r\nContent-Length: 14\r\nContent-Type: text/plain\r\n\r\n";
//   string expected_response = response_protocol + request;

//   rep.content = request;
//   buffer = rep.to_buffers();
//   string res_response = convert_buffer(buffer);
  
//   bool success = (expected_response == res_response);

//   EXPECT_TRUE(success);
// }