#include <iostream>
#include <cstdlib>
#include "file_manager.h"
#include <filesystem>

using namespace std;

bool createZipFromFolder(const std::string& folderPath) {
    std::string command = "./create_zip.sh \"" + folderPath + "\"";  
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Error: Failed to create ZIP file for " << folderPath << std::endl;
    }
    return (result == 0);
}

bool copyFolder(const std::string& folderPath) {
    std::string command = "./copy_folder.sh \"" + folderPath + "\"";
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Error: Failed to copy folder " << folderPath << std::endl;
    }
    return (result == 0);
}
bool deleteFile(const std::string& filePath) {
    try {
        if (fs::remove(filePath)) {
            return true;
        } else {
            std::cerr << "Error: Failed to delete file " << filePath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error deleting file: " << e.what() << std::endl;
        return false;
    }
}