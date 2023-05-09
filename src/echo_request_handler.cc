#include <string>

<<<<<<< HEAD
reply Echo_Request_Handler::handleRequest(char* data, int bytes_transferred){
  using namespace std;
  reply_.status = reply::ok;
=======
#include "echo_request_handler.h"
#include "reply.h"

using namespace std;

reply Echo_Request_Handler::handleRequest(char* data, int bytes_transferred, reply::status_type status)
{
  reply_.status = status;
>>>>>>> 187db93 (Code cleanup and additional logging)
  reply_.content = string(data, bytes_transferred);
  header length_header = {"Content-Length", to_string(bytes_transferred)};
  header type_header = {"Content-Type", "text/plain"};
  reply_.headers = {length_header, type_header};
  return reply_;
}
