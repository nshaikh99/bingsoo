#ifndef REAL_FILESYSTEM_H
#define REAL_FILESYSTEM_H

#include <filesystem>
#include <fstream>
#include "filesystem_interface.h"

class real_filesystem : public filesystem_interface {
public:
    bool file_exists(const std::string& filename) const override;
    bool read_file(const std::string& filename, std::string& contents) const override;
    void write_file(const std::string& filename, const std::string& contents) override;
    bool delete_file(const std::string& filename) override;
    std::vector<std::string> list_files(const std::string& path) override;

};


#endif // REAL_FILESYSTEM_H