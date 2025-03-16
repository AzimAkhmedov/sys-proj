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