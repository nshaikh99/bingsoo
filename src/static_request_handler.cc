#include "static_request_handler.h"
#include <string>
#include <fstream>
#include <iostream>

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

Static_Request_Handler::Static_Request_Handler(std::string file_path){
    file_path_ = file_path;
}

reply Static_Request_Handler::getStaticReply(char* data, size_t bytes_transferred, reply::status_type status){
    std::ifstream static_file(file_path_.c_str(), std::ios::in | std::ios::binary);
    if (!static_file)
    {
        //// TODO: REPLACE THIS WITH A 404 ERROR
        std::cerr << "no file: " << file_path_ << std::endl;
        reply_.status = status;
        reply_.content = std::string(data, bytes_transferred);
        header length_header = {"Content-Length", std::to_string(bytes_transferred)};
        header type_header = {"Content-Type", "text/plain"};
        reply_.headers = {length_header, type_header};
        return reply_;
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

    reply_.status = status;
    reply_.content = file_contents;
    header length_header = {"Content-Length", std::to_string(file_contents.length())};
    header type_header = {"Content-Type", extension_type_map(ext_type)};
    reply_.headers = {length_header, type_header};
    std::cout << "serving file: " << ext_type << std::endl;
    std::cout << "serving file: " << extension_type_map(ext_type) << std::endl;
    return reply_;
}

