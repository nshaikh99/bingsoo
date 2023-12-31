// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "config_parser.h"

#include "global.h"

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        *value += c;
        if (c == '\'') {
          const char after_c = input->get();
          if (!input->good()) {
            break;
          }
          if (after_c){
            if (after_c == ' ' || after_c == '\t' || after_c == '\n' || after_c == '\r' || 
                  after_c == '{' || after_c == '}' || after_c == ';'){
              input->unget();
              return TOKEN_TYPE_NORMAL;
            }
          }
          return TOKEN_TYPE_ERROR;
        }
        else if (c == '\\'){
          value->pop_back();
          *value += c;
          state = TOKEN_STATE_SINGLE_QUOTE;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          const char after_c = input->get();
          if (!input->good()) {
            break;
          }
          if (after_c){
            if (after_c == ' ' || after_c == '\t' || after_c == '\n' || after_c == '\r' || 
                  after_c == '{' || after_c == '}' || after_c == ';'){
              input->unget();
              return TOKEN_TYPE_NORMAL;
            }
          }
          return TOKEN_TYPE_ERROR;
        }
        else if (c == '\\'){
          value->pop_back();
          *value += c;
          state = TOKEN_STATE_DOUBLE_QUOTE;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  int block_start_count = 0;
  int block_end_count = 0;
  BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "[ConfigParsing]" << "\n----BEGIN CONFIG PARSING----\n";
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);

    // log each token in the config file
    BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Parsing " << TokenTypeAsString(token_type) << ": " << token.c_str();

    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      block_start_count++;
      block_end_count--;
      if (last_token_type != TOKEN_TYPE_NORMAL && last_token_type != TOKEN_TYPE_START_BLOCK) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      block_start_count--;
      block_end_count++;
      if (last_token_type != TOKEN_TYPE_STATEMENT_END  && last_token_type != TOKEN_TYPE_START_BLOCK && last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.
        break;
      }
      if (block_start_count != 0){
        break;
      }
      if (block_end_count != 0){
        break;
      }
      BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "\n----END OF CONFIG PARSING----\n";
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }
  BOOST_LOG_TRIVIAL(error) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::ERROR] << "Error parsing config file: " << TokenTypeAsString(token_type) << " followed " << TokenTypeAsString(last_token_type);
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    // printf ("Failed to open config file: %s\n", file_name);
    BOOST_LOG_TRIVIAL(error) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::ERROR] << "Failed to open config file: " << file_name;

    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}

int NginxConfig::get_port_num(){
  for (auto config_statement : statements_) {
    if (config_statement->child_block_.get() == nullptr) {
      if (config_statement->tokens_[0] == "port") {
        // if the line says "port <port_num>"
        int port_number = atoi(config_statement->tokens_[1].c_str());
        if (port_number >= 0 && port_number <= 65535){
          // if inside the valid port number range
          return port_number;
        }
        else {
          return -1;
        }
      }
    }
    else if (config_statement->child_block_.get() != nullptr) {
      // recurse through nested blocks to find the inner block with a port number
      int port_num = config_statement -> child_block_ -> get_port_num();
      if (port_num != -1)
        return port_num;
    }
  }
  return -1;
}

std::unordered_map<std::string,std::string> NginxConfig::get_static_file_path(){
  std::unordered_map<std::string,std::string> paths_map;
  BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Building list of static routes";
  for (const auto& statement : statements_) {
    if (statement->tokens_[0] == "location" && statement->tokens_[2] == "StaticHandler") {
      for (auto config_statement : statement -> child_block_ -> statements_) {
        int token_length = config_statement->tokens_.size();
        for (int i = 0; i < token_length; i++){
          if (config_statement->tokens_[i] == "root") {
            std::string result_file_path = config_statement->tokens_[i+1];
            size_t extension_start = result_file_path.find_last_of("/");
            std::string built_uri;
            if (extension_start != std::string::npos)
                built_uri = statement->tokens_[1] + result_file_path.substr(extension_start);
            else {
              built_uri = statement->tokens_[1] + "/" 
              + result_file_path;
            }
            paths_map[built_uri] = result_file_path;
            BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Static File: " << result_file_path;
            BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Built URI: " << built_uri;
          }
        }
      }
      return paths_map;
    }
  }
  return paths_map;
}

std::string NginxConfig::get_static_serving_path(){
  std::string paths_str;
  for (const auto& statement : statements_) {
    if (statement->tokens_[0] == "location" && statement->tokens_[2] == "StaticHandler") {
      paths_str = statement->tokens_[1];
      return paths_str;
    }
  }
  return paths_str;
}

std::string NginxConfig::get_echo_path(){
  std::string paths_str;
  for (const auto& statement : statements_) {
    if (statement->tokens_[0] == "location" && statement->tokens_[2] == "EchoHandler") {
      paths_str = statement->tokens_[1];
      return paths_str;
    }
  }
  return paths_str;
}

std::string NginxConfig::get_health_path(){
  std::string paths_str;
  for (const auto& statement : statements_) {
    if (statement->tokens_[0] == "location" && statement->tokens_[2] == "HealthHandler") {
      paths_str = statement->tokens_[1];
      return paths_str;
    }
  }
  return paths_str;
}

std::string NginxConfig::get_sleep_path(){
  std::string paths_str;
  for (const auto& statement : statements_) {
    if (statement->tokens_[0] == "location" && statement->tokens_[2] == "SleepHandler") {
      paths_str = statement->tokens_[1];
      return paths_str;
    }
  }
  return paths_str;
}
 
std::string NginxConfig::get_crud_path() {
  std::string path_str;

  for(const auto& statement : statements_) {
    if(statement->tokens_[0] == "location" && statement->tokens_[2] == "CrudHandler") {
        path_str = statement->tokens_[1];
        BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Obtained CRUD path: " << path_str;
    }
  }
  return path_str;
}

std::string NginxConfig::get_markdown_path(){
  std::string paths_str;
  for (const auto& statement : statements_) {
    if (statement->tokens_[0] == "location" && statement->tokens_[2] == "MarkdownHandler") {
      paths_str = statement->tokens_[1];
      return paths_str;
    }
  }
  return paths_str;
}

std::unordered_map<std::string,std::string> NginxConfig::get_crud_args() {
  std::unordered_map<std::string,std::string> arg_map;

  for(const auto& statement : statements_) {
    if(statement->tokens_[0] == "location" && statement->tokens_[2] == "CrudHandler") {
        for (auto config_statement : statement -> child_block_ -> statements_) {
            if (!config_statement->tokens_[0].empty()) {
              arg_map[config_statement->tokens_[0]] = config_statement->tokens_[1];
              BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Obtained " << config_statement->tokens_[0] << " arg: " << config_statement->tokens_[1];
          }
        }
      }
    }
  return arg_map;
}

std::unordered_map<std::string,std::string> NginxConfig::get_markdown_args() {
  std::unordered_map<std::string,std::string> arg_map;

  for(const auto& statement : statements_) {
    if(statement->tokens_[0] == "location" && statement->tokens_[2] == "MarkdownHandler") {
        for (auto config_statement : statement -> child_block_ -> statements_) {
            if (!config_statement->tokens_[0].empty()) {
              arg_map[config_statement->tokens_[0]] = config_statement->tokens_[1];
              BOOST_LOG_TRIVIAL(info) << LOG_MESSAGE_TYPES[LOG_MESSAGE_TYPE::INFO] << "Obtained " << config_statement->tokens_[0] << " arg: " << config_statement->tokens_[1];
          }
        }
      }
    }
  return arg_map;
}