#include "static_request_handler.h"
#include "reply.h"
#include <string>
#include <fstream>
#include <iostream>


Static_Request_Handler::Static_Request_Handler(std::string file_path){
    file_path_ = "." + file_path; //"." is added in order to root the directory. path will not be found without it
}

status Static_Request_Handler::handleRequest(const request req, response& res){
    std::ifstream static_file(file_path_.c_str(), std::ios::in | std::ios::binary);
    if (!static_file)
    {
        beast::ostream(res.body()) << reply_.stock_reply(reply::not_found);
        res.result(http::status::not_found);
        res.content_length((res.body().size()));
        res.set(http::field::content_type, "text/html");
        return res.result();
    }

    // Find the extension type and map it to the return content type
    std::string ext_type;
    size_t extension_start = file_path_.find_last_of(".");
    if (extension_start != std::string::npos)
        ext_type = file_path_.substr(extension_start);

    // Read the contents of the file
    char c;
    std::string file_contents;
    while (static_file.get(c))
    {
        file_contents += c;
    }
    static_file.close();

    // Populate fields
    beast::ostream(res.body()) << file_contents;
    res.result(http::status::ok);
    res.content_length((res.body().size()));
    res.set(http::field::content_type, extension_type_map(ext_type));
    return res.result();
}

std::string extension_type_map(std::string ext)
{
    if(ext==".htm")  return "text/html";
    if(ext==".html") return "text/html";
    if(ext==".php")  return "text/html";
    if(ext==".css")  return "text/css";
    if(ext==".txt")  return "text/plain";
    if(ext==".js")   return "application/javascript";
    if(ext==".json") return "application/json";
    if(ext==".xml")  return "application/xml";
    if(ext==".swf")  return "application/x-shockwave-flash";
    if(ext==".flv")  return "video/x-flv";
    if(ext==".png")  return "image/png";
    if(ext==".jpe")  return "image/jpeg";
    if(ext==".jpeg") return "image/jpeg";
    if(ext==".jpg")  return "image/jpeg";
    if(ext==".gif")  return "image/gif";
    if(ext==".bmp")  return "image/bmp";
    if(ext==".ico")  return "image/vnd.microsoft.icon";
    if(ext==".tiff") return "image/tiff";
    if(ext==".tif")  return "image/tiff";
    if(ext==".svg")  return "image/svg+xml";
    if(ext==".svgz") return "image/svg+xml";
    if(ext==".zip") return "application/zip";
    if(ext==".pdf") return "application/pdf";
    return "application/text";
}