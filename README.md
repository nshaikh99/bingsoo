# bingsoo README

## Table of contents
1. [Source code layout](#source-code-layout)
2. [Build, test, and run](#build-test-and-run)
    * [Build](#build)
    * [Test](#test)
    * [Run](#run)
    * [Config file format](#config-file-format)
3. [How to add a request handler](#how-to-add-a-request-handler)
    * [Design and implement request handler](#define-and-implement-request-handler)
    * [Define and implement factory](#define-and-implement-factory)
    * [Add to config file](#add-to-config-file)
    * [Add to config parser interface and implementation](#add-to-config-parser-interface-and-implementation)
    * [Add to server](#add-to-server)

## Source code layout
The source code for this project is laid out as follows:
```
                    +----------------+
                    |      Main      |
                    +----------------+
                            ||
                    +----------------+
         +----------|     Server     |----------+
         |          +----------------+          |
         |                  ||                  |
+-----------------+ +-----------------+ +-----------------+
|  Config parser  | |     Session     | |    Factories    |
+-----------------+ +-----------------+ +-----------------+
                            ||                  |
                    +----------------+  +-----------------+
                    | Request parser |  | Request handler |
                    +----------------+  +-----------------+
```
Main instantiates a server, which in turn does three things: first, it creates a config parser and parses the config file. Next, it creates factories for the different handlers specified in the config file. Finally, it creates a session for each request that it receives. Each factory instantiates its own request handler and each session instantiates its own request parser that handles the incoming request. It is worth highlighting that the server passes the parsed config object from the config parser to the factories so that they know which handlers to create, and it also passes the factories (and implicitly the request handlers associated with them) to each session.

## Build, test, and run

### Build
In-source builds are not allowed for this project in order to avoid cluttering the source code with build files. The following commands should be executed from the root directory of the project to build source code from a fresh clone of this repository:
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```
The following commands should be executed from the root directory of the project to build source code for all subsequent builds:
```sh
$ cd build
$ make
```

### Test
Once source code has been built, the following commands should be executed from the root directory of the project to run tests:
```sh
$ cd build
$ make test
```
`make test` will output a summary of which tests (if any) failed after the test suite runs.

### Run
Once source code has been built and tested, the following commands should be executed from the root directory of the project to run the code:
```sh
$ build/bin/server [configuration file]
```
where `[configuration file]` is a config file compliant with the format (more on this later). A sample config file with the path `conf/sample.conf` is provided.

### Config file format
A sample config file is shown below:
```apacheconf
port 80;

location /echo EchoHandler {}

location /static StaticHandler {
    root static/index.html
    root static/screenshot1.png
}
```
The `port` field defines which port the server should run on (in this case, port 80). The first `location` block specifies that requests to the `/echo` path should be handled by an EchoHandler, while the second specifies that requests to the `/static` path should be handled by a StaticHandler. The StaticHandler will serve `static/index.html` or `static/screenshot1.png` from the root directory of the project depending on whether `index.html` or `screenshot1.png` is appended to the `/static/` path.

## How to add a request handler
Unless otherwise stated, it is generally considered good practice to put all header files in the `include` folder, source files in the `src` folder, and config files in the `conf` folder.

### Define and implement request handler
All request handlers should inherit from the `RequestHandler` base class and override the `handleRequest` method. A sample request handler interface is shown below:
```cpp
#ifndef sample_handler_h
#define sample_handler_h

#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

typedef bool status;

class SampleHandler : public RequestHandler {
 public:
  status handle_request(
    const http::request<http::string_body> req,
    http::response<http::string_body> res
  );
};

#endif
```
A sample request handler interface implementation is shown below:
```cpp
#include "sample_handler.h"

status SampleHandler::handle_request(
  const http::request<http::string_body> req,
  http::response<http::string_body> res
)
{
  // implement handle_request method here
}

// add helper functions here
```

### Define and implement factory
All request handlers need to have a factory with which they are associated. A sample request handler factory interface is shown below:
```cpp
#ifndef sample_handler_factory_h
#define sample_handler_factory_h

#include "request_handler_factory.h"

class SampleHandlerFactory : public RequestHandlerFactory {
 public:
  SampleHandlerFactory(
    const std::string& path,
    const NginxConfig& config
  );
  RequestHandler* create();
 private:
  std::string path_;
  NginxConfig config_;
};

#endif
```

A sample request handler factory interface implementation is shown below:
```cpp
#include "sample_handler_factory.h"
#include "sample_handler.h"

SampleHandlerFactory::SampleHandlerFactory(
  const std::string& path,
  const NginxConfig& config
) : path_(path), config_(config) {}

RequestHandler* SampleHandlerFactory::create() {
  return new SampleHandler();
}

```

### Add to config file
The config file must be updated to use the new handler. A sample config file containing a sample request handler is shown below:
```apacheconf
port 80;

location /echo EchoHandler {}

location /static StaticHandler {
    root static/index.html
    root static/screenshot1.png
}

location /sample SampleHandler {}
```

### Add to config parser interface and implementation
The config parser must be updated to be able to identify the new handler in a config file. A possible addition (for the sample request handler) to the config parser interface is shown below:
```cpp
// in config_parser.h

class NginxConfig {
 public:
  ...
  std::string get_sample_path();
}
```

A possible addition (for the sample request handler) to the config parser interface implementation is shown below:
```cpp
// in config_parser.cc

std::string NginxConfig::get_sample_path() {
  std::string paths_str;
  for(const auto& statement : statements_) {
    if(statements->tokens_[0] == "location" &&
       statements->tokens_[2] == "SampleHandler")
    {
        paths_str = statement->tokens_[1];
        return paths_str;
    }
  }
  return paths_str;
}
```

### Add to server
The server must be updated so that it is aware of what factories are available to deploy to a session. A possible addition (for the sample request handler) to the server implementation is shown below:
```cpp
// in server.cc

RequestHandlerFactory* server::createHandlerFactory(
  const string& name,
  string uri
)
{
  if(name == "StaticHandler")
    return new StaticHandlerFactory(uri, config_);
  else if(name == "EchoHandler")
    return new EchoHandlerFactory(uri, config_);
  
  // add the following else statement
  else if(name == "SampleHandler")
    return new SampleHandlerFactory(uri, config_);
  
  else
    return new _404HandlerFactory(uri, config_);
}

std::unordered_map<std::string, RequestHandlerFactory*> server::configMap() {
  std::unordered_map<std::string, RequestHandlerFactory*> routes;
  std::unordered_map<std::string, std::string> static_path_map = config_.get_static_file_path();
  std::string echo_path = config_.get_echo_path();
  for(auto it : static_path_map) {
    routes[it.first] = createHandlerFactory("StaticHandler", it.first);
  }
  routes[echo_path] = createHandlerFactory("EchoHandler", echo_path);

  // add the following lines
  std::string sample_path = config_.get_sample_path();
  routes[sample_path] = createHandlerFactory("SampleHandler", sample_path);

  routes["/"] = createHandlerFactory("_404Handler", "/");
  return routes;
}
```
