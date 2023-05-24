#include "gtest/gtest.h"
#include <boost/beast/http/status.hpp>

#include "crud_request_handler.h" 
#include "mock_filesystem.h"

using namespace boost::beast::http;
TEST(MockFilesystemTest, FileExists) {
    mock_filesystem fs;
    // Add a file to the mock filesystem
    fs.write_file("path/to/file", "File contents");
    // Test if the file exists in mock filesystem
    EXPECT_TRUE(fs.file_exists("path/to/file"));
    EXPECT_FALSE(fs.file_exists("nonexistent/file"));
}

TEST(MockFilesystemTest, ReadFile) {
    mock_filesystem fs;
    // Add a file to the filesystem
    fs.write_file("path/to/file.txt", "File contents");
    std::string contents;
    // Test reading an existing file
    EXPECT_TRUE(fs.read_file("path/to/file.txt", contents));
    EXPECT_EQ(contents, "File contents");
    // Test reading a non-existing file
    contents = "";
    EXPECT_FALSE(fs.read_file("nonexistent/file.txt", contents));
    EXPECT_EQ(contents, "");
}

TEST(MockFilesystemTest, WriteFile) {
    mock_filesystem fs;
    // Write a file to the filesystem
    fs.write_file("path/to/file.txt", "File contents");
    std::string contents;
    // Test if the file was written successfully
    EXPECT_TRUE(fs.file_exists("path/to/file.txt"));
    EXPECT_TRUE(fs.read_file("path/to/file.txt", contents));
    EXPECT_EQ(contents, "File contents");
}
TEST(MockFilesystemTest, DeleteFile) {
    mock_filesystem fs;
    // Add a file to the filesystem
    fs.write_file("path/to/file", "File contents");
    // Test deleting an existing file
    EXPECT_TRUE(fs.delete_file("path/to/file"));
    EXPECT_FALSE(fs.file_exists("path/to/file"));
    // Test deleting a non-existing file
    EXPECT_FALSE(fs.delete_file("nonexistent/file"));
}

TEST(MockFilesystemTest, ListFiles) {
    mock_filesystem fs;
    // Add  files to the filesystem
    fs.write_file("path/to/file1.txt", "File 1 contents");
    fs.write_file("path/to/file2.txt", "File 2 contents");
    fs.write_file("path/to/dir/file3.txt", "File 3 contents");
    // Test listing files in a directory
    std::vector<std::string> files = fs.list_files("path/to");
    EXPECT_EQ(files.size(), 3);
    std::vector<std::string> output = {"file1", "file2", "dir"};
    EXPECT_EQ(files, output);
}

TEST(RequestCrudTest, PostRequest) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Set the target of the POST request
    std::string target = "./mnt/crud/Shoes";
    std::string requestBody = "{\"name\": \"New Shoes data\"}";
    // Create a POST request for the directory
    http::request<http::string_body> request;
    request.method(http::verb::post);
    request.target("/api/Shoes");
    request.body() = requestBody;
    // Call the handle_request function
    bool success = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_TRUE(success);
    
    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::created);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    // Assert the response body (newly created entity ID)
    ASSERT_EQ(response.body(), "{\"id\": 1}");
    // Assert that the file was written with the correct content
    std::string createdFile = target + "/1";
    std::string contents;
    m_filesystem->read_file(createdFile, contents);
}


TEST(RequestCrudTest, GetFile) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/api/Shoes/1");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_TRUE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::ok);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), fileContent);
}


TEST(RequestCrudTest, PostFile) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::post);
    request.target("/api/Shoes");  // Set the target path of the request
    request.body() = "{\n    \"brand\": \"Adidas\",\n    \"color\": \"light-green\",\n  \"size\": \"12\"}";
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_TRUE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::created);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "{\"id\": 2}");
}

TEST(RequestCrudTest, PutFileToExistingId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::put);
    request.target("/api/Shoes/1");  // Set the target path of the request
    request.body() = "{\n    \"brand\": \"Adidas\",\n    \"color\": \"light-green\",\n  \"size\": \"12\"}";
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_TRUE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::ok);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "Edited ./mnt/crud/Shoes/1 on the server");
}

TEST(RequestCrudTest, PutFileToDesiredId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::put);
    request.target("/api/Shoes/3");  // Set the target path of the request
    request.body() = "{\n    \"brand\": \"Adidas\",\n    \"color\": \"light-green\",\n  \"size\": \"12\"}";
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_TRUE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::created);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "Edited ./mnt/crud/Shoes/3 on the server");
}

TEST(RequestCrudTest, DeleteFile) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::delete_);
    request.target("/api/Shoes/1");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_TRUE(status);
    ASSERT_FALSE(m_filesystem->file_exists(target));

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::ok);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), fileContent);
}

TEST(RequestCrudTest, BadRequestWithOneSlash) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/apiShoes1");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::bad_request);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "Request uri /apiShoes1 only has 1 slash, instead of 2");
}

TEST(RequestCrudTest, PostRequestWithId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::post);
    request.target("/api/Shoes/1");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::bad_request);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "id in request uri of post request");
}

TEST(RequestCrudTest, GetRequestWithIncorrectDirectory) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/api/Caps");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::not_found);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "Directory not found");
}

TEST(RequestCrudTest, GetRequestWithIncorrectId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::get);
    request.target("/api/Shoes/85");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::not_found);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "File does not exist");
}

TEST(RequestCrudTest, DeleteRequestWithIncorrectId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::delete_);
    request.target("/api/Shoes/85");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::not_found);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "File does not exist");
}

TEST(RequestCrudTest, DeleteRequestWithNoId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::delete_);
    request.target("/api/Shoes/a1b2");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::bad_request);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "numerical id not in request uri of delete request");
}

TEST(RequestCrudTest, PutRequestWithNoId) {
    // Create a mock filesystem
    std::shared_ptr<filesystem_interface> m_filesystem = std::make_shared<mock_filesystem>();
    // Create a response object
    http::response<http::string_body> response;
    // Create an instance of the request_crud class
    CrudHandler crud = CrudHandler("/mnt/crud", m_filesystem);
    // Write a test file to the mock filesystem for the requested file
    std::string target = " ./mnt/crud/Shoes/1";
    std::string fileContent = "{\n    \"brand\": \"Nike\",\n    \"color\": \"red\"\n}";
    m_filesystem->write_file(target, fileContent);
    // Create a GET request for the test file
    http::request<http::string_body> request;
    request.method(http::verb::put);
    request.target("/api/Shoes/a1b2");  // Set the target path of the request
    // Call the handle_request function
    bool status = crud.handle_request(request, response);
    // Assert the expected result
    ASSERT_FALSE(status);

    // Assert the response properties
    ASSERT_EQ(response.result(), http::status::bad_request);
    ASSERT_EQ(response[boost::beast::http::field::content_type], "application/json");
    ASSERT_EQ(response.body(), "numerical id not in request uri of put request");
}