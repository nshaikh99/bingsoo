#ifndef MOCK_FILESYSTEM_H
#define MOCK_FILESYSTEM_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "filesystem_interface.h"
using namespace boost::property_tree;

class mock_filesystem : public filesystem_interface {
public:
    bool file_exists(const std::string& filename) const override;
    bool read_file(const std::string& filename, std::string& contents) const override;
    void write_file(const std::string& filename, const std::string& contents) override;
    bool delete_file(const std::string& filename) override;
    std::vector<std::string> list_files(const std::string& path) override;
    void display_filesystem() const;
private:
  std::string to_mock_format(const std::string& filename) const;
  ptree mock_system_;
};


#endif // MOCK_FILESYSTEM_H