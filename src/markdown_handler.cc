#include "markdown_handler.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// hi nabeel - andy (hi muthu ther reviewer, remove this if you see this)

MarkdownHandler::MarkdownHandler(std::string data_path) : data_path_(data_path) {}

status MarkdownHandler::handle_request(const http::request<http::string_body> req, http::response<http::string_body> &res) {
  std::string md_file_path = std::string(req.target());
  md_file_path = md_file_path.substr(md_file_path.find_first_of('/') + 1);

  // if only one / then empty file path which means README
  if(md_file_path.find_first_of('/') == std::string::npos) {
    md_file_path = "README.md";
  }
  else {
    md_file_path = md_file_path.substr(md_file_path.find_first_of('/') + 1);
    md_file_path = data_path_[data_path_.size() - 1] == '/' ? data_path_ + md_file_path : data_path_ + '/' + md_file_path;
  }
  
  // check for .md file extension
  if(!boost::ends_with(md_file_path, ".md")) {
    res.body() = "File requested is not a markdown file\n";
    res.result(http::status::bad_request);
    res.content_length(res.body().size());
    res.set(http::field::content_type, "text/plain");
    return false;
  }

  // check that file exists
  if(!boost::filesystem::exists(md_file_path)) {
    res.body() = "File does not exist\n";
    res.result(http::status::not_found);
    res.content_length(res.body().size());
    res.set(http::field::content_type, "text/plain");
    return false;
  }

  std::ifstream md_file(md_file_path);
  
  // check that file successfully opened
  if(!md_file.good()) {
    res.body() = "Error opening file\n";
    res.result(http::status::internal_server_error);
    res.content_length(res.body().size());
    res.set(http::field::content_type, "text/plain");
    return false;
  }

  std::string line;
  std::vector<std::string> lines;

  // read file
  while(std::getline(md_file, line)) {
    lines.push_back(line);
  }
  md_file.close();

  // render HTML equivalent
  std::string html_file_path = md_file_path.substr(0, md_file_path.size() - 3) + ".html";

  // if an HTML equivalent already exists then delete it
  if(boost::filesystem::exists(html_file_path)) {
    if(std::remove(&html_file_path[0])) {
      res.body() = "Error deleting file\n";
      res.result(http::status::internal_server_error);
      res.content_length(res.body().size());
      res.set(http::field::content_type, "text/plain");
      return false;
    }
  }
  
  std::ofstream html_file(html_file_path, std::ios_base::app);
  bool inside_code_snippet = false;
  for(int i = 0; i < lines.size(); i++) {
    // bold
    while(lines[i].find("**") != std::string::npos || lines[i].find("__") != std::string::npos) {
      std::string new_line = lines[i];
      int idx;
      if(lines[i].find("**") != std::string::npos) {
        idx = new_line.find("**");
        new_line.replace(idx, 2, "<strong>");
        idx = new_line.find("**");
        if(idx != std::string::npos) {
          new_line.replace(idx, 2, "</strong>");
          lines[i] = new_line;
        }
        else {
          break;
        }
      }
      else {
        idx = new_line.find("__");
        new_line.replace(idx, 2, "<strong>");
        idx = new_line.find("__");
        if(idx != std::string::npos) {
          new_line.replace(idx, 2, "</strong>");
          lines[i] = new_line;
        }
        else {
          break;
        }
      }
    }

    // italics
    while(lines[i].find("*") != std::string::npos || lines[i].find("_") != std::string::npos) {
      std::string new_line = lines[i];
      int idx;
      if(lines[i].find("*") != std::string::npos) {
        idx = new_line.find("*");
        new_line.replace(idx, 1, "<em>");
        idx = new_line.find("*");
        if(idx != std::string::npos) {
          new_line.replace(idx, 1, "</em>");
          lines[i] = new_line;
        }
        else {
          break;
        }
      }
      else {
        idx = new_line.find("_");
        new_line.replace(idx, 1, "<em>");
        idx = new_line.find("_");
        if(idx != std::string::npos) {
          new_line.replace(idx, 1, "</em>");
          lines[i] = new_line;
        }
        else {
          break;
        }
      }
    }

    // in-line code
    while(lines[i].find("`") != std::string::npos && lines[i].find("```") == std::string::npos) {
      std::string new_line = lines[i];
      int idx;
      idx = new_line.find("`");
      new_line.replace(idx, 1, "<code>");
      idx = new_line.find("`");
      if(idx != std::string::npos) {
        new_line.replace(idx, 1, "</code>");
        lines[i] = new_line;
      }
      else {
        break;
      }
    }

    // small headings
    if(lines[i].size() >= 4 && lines[i].substr(0, 3) == "###" && lines[i][3] == ' ') {
      html_file << "<h3 style=\"white-space: pre\">" + lines[i].substr(4) + "</h3>" << std::endl;
    }

    // medium headings
    else if(lines[i].size() >= 3 && lines[i].substr(0, 2) == "##" && lines[i][2] == ' ') {
      html_file << "<h2 style=\"white-space: pre\">" + lines[i].substr(3) + "</h2>" << std::endl;
    }

    // large headings
    else if(lines[i].size() >= 2 && lines[i].substr(0, 1) == "#" && lines[i][1] == ' ') {
      html_file << "<h1 style=\"white-space: pre\">" + lines[i].substr(2) + "</h1>" << std::endl;
    }

    // block code
    else if(lines[i].size() >= 3 && lines[i].substr(0, 3) == "```") {
      if(inside_code_snippet) {
        inside_code_snippet = !inside_code_snippet;
        html_file << "</code></p>" << std::endl;
      }
      else {
        inside_code_snippet = !inside_code_snippet;
        html_file << "<p style=\"white-space: pre\"><code>";
      }
    }

    // everything else
    else {
      html_file << "<p style=\"white-space: pre\">" + lines[i] + "</p>" << std::endl;
    }
  }
  html_file.close();

  res.body() = "";

  std::ifstream html_file_2(html_file_path);
  
  // check that file successfully opened
  if(!html_file_2.good()) {
    res.body() = "Error opening file\n";
    res.result(http::status::internal_server_error);
    res.content_length(res.body().size());
    res.set(http::field::content_type, "text/plain");
    return false;
  }

  // read file
  while(std::getline(html_file_2, line)) {
    res.body() += line;
  }
  html_file_2.close();
  
  res.result(http::status::ok);
  res.content_length(res.body().size());
  res.set(http::field::content_type, "text/html");
  return true;
}
