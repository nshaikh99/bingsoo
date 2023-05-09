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