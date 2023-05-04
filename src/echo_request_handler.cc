#include "echo_request_handler.h"
#include "reply.h"
#include <string>

Echo_Request_Handler::Echo_Request_Handler() = default;


reply Echo_Request_Handler::handleRequest(char* data, int bytes_transferred, reply::status_type status){
  using namespace std;
  reply_.status = status;
  reply_.content = string(data, bytes_transferred);
  header length_header = {"Content-Length", to_string(bytes_transferred)};
  header type_header = {"Content-Type", "text/plain"};
  reply_.headers = {length_header, type_header};
  return reply_;
}