#ifndef FILESYSTEM_INTERFACE_H
#define FILESYSTEM_INTERFACE_H

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <vector>

class filesystem_interface {
public:
    virtual bool file_exists(const std::string& filename) const = 0;
    virtual bool read_file(const std::string& filename, std::string& contents) const = 0;
    virtual void write_file(const std::string& filename, const std::string& contents) = 0;
    virtual bool delete_file(const std::string& filename) = 0;
    virtual std::vector<std::string> list_files(const std::string& path) = 0;
};


#endif // FILESYSTEM_INTERFACE_H