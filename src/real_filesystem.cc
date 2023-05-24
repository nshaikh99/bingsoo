#include "real_filesystem.h"


bool real_filesystem::file_exists(const std::string& filename) const {
    // Check if the specified file exists in the filesystem
    // For example, if the file is directory/file.txt, see if such a file exists in the root
    // directory
    return std::filesystem::exists(filename);
}

bool real_filesystem::read_file(const std::string& filename, std::string& contents) const {
    // Read the contents of the specified file into the contents string
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            contents += line;
            if (file.good()) {
              contents += '\n';
            }
        }
        file.close();
        return true;
    }
    return false;
}

void real_filesystem::write_file(const std::string& filename, const std::string& contents) {
    std::filesystem::path pathname(filename);
    if (!pathname.parent_path().empty() && !std::filesystem::exists(pathname.parent_path())) {
    // Create the directories leading up to the file if they don't exist
      std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
    } 
    // Write the contents string to the specified file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << contents;
        file.close();
    }
}

bool real_filesystem::delete_file(const std::string& filename) {
    // Delete the specified file
    return std::filesystem::remove(filename);
}

// from the directory of path that is a string, loop through each file in the directory and add it to a vector
// then return the vector
std::vector<std::string> real_filesystem::list_files(const std::string& path) {
    
    // Create a JSON array to store the file names
    std::vector<std::string> fileNames;

    // Iterate through the files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        // Get the file name
        std::string fileName = entry.path().filename().string();
        // Add the file name to the JSON array
        fileNames.push_back(fileName);
    }
    return fileNames;
}