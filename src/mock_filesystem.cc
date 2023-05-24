#include "mock_filesystem.h"

bool mock_filesystem::file_exists(const std::string& filename) const {
    std::string converted = to_mock_format(filename);
    boost::optional<const boost::property_tree::ptree&> node = mock_system_.get_child_optional(converted);
    return node.is_initialized(); // Returns true if the file exists, false otherwise
}
bool mock_filesystem::read_file(const std::string& filename, std::string& contents) const {
    std::string converted = to_mock_format(filename);
    if (file_exists(converted)) {
        contents = mock_system_.get<std::string>(converted);
        return true;
    }
    return false;
}
void mock_filesystem::write_file(const std::string& filename, const std::string& contents) {
    std::string converted = to_mock_format(filename);
    mock_system_.put(converted, contents);
}
bool mock_filesystem::delete_file(const std::string& filename) {
    std::string converted = to_mock_format(filename);
    if (file_exists(converted)) {
        // Extract the last directory in the filepath
        std::size_t lastDotPos = converted.find_last_of('.');
        std::string parentPath = converted.substr(0, lastDotPos);
        std::string lastDir = converted.substr(lastDotPos + 1);
        // Erase the last directory from the property tree
        mock_system_.get_child(parentPath).erase(lastDir);
        display_filesystem();
        return true;
    }
    return false;
}
void mock_filesystem::display_filesystem() const {
    // For ease of debugging during testing. Never runs when server is on.
    std::ostringstream oss;
    boost::property_tree::write_json(oss, this->mock_system_);
    std::cout << oss.str() << std::endl;
}
std::string mock_filesystem::to_mock_format(const std::string& filename) const {
    // Converts filenames to format suitable for ptree mocksystem 
    // All slashes convert to dots in filename
    std::string converted = filename;
    std::string searchString = "../";
    size_t pos = converted.find(searchString);
    while (pos != std::string::npos) {
        converted.replace(pos, searchString.length(), "");
        pos = converted.find(searchString, pos);
    }
    std::replace(converted.begin(), converted.end(), '/', '.');
    converted.erase(std::remove(converted.begin(), converted.end(), ' '), converted.end());
    std::cout<<"converted"<<converted<<std::endl;
    return converted;
}
std::vector<std::string> mock_filesystem::list_files(const std::string& path) {
    std::string converted = to_mock_format(path);
    std::vector<std::string> fileNames;
    display_filesystem();
    for (const auto& entry : mock_system_.get_child(converted)) {
        std::string fileName = entry.first;
        fileNames.push_back(fileName);
        std::cout<<"filename: "<<fileName<<std::endl;
    }
    return fileNames;
}