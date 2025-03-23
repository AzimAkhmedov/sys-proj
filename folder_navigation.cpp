#include "folder_navigation.h"
#include <iostream>
#include <unistd.h>
#include <limits.h>

FolderNavigation::FolderNavigation() {
    char buffer[PATH_MAX];
    getcwd(buffer, sizeof(buffer));
    currentPath = buffer;
    breadcrumbTrail.push_back(currentPath);
}

void FolderNavigation::navigateTo(const std::string& dir_name) {
    if (dir_name == "..") {
        goBack();
        return;
    }

    std::string newPath = currentPath + "/" + dir_name;
    if (chdir(newPath.c_str()) == 0) {
        char buffer[PATH_MAX];
        getcwd(buffer, sizeof(buffer));
        currentPath = buffer;
        breadcrumbTrail.push_back(currentPath);
    } else {
        std::cerr << "Failed to navigate to " << dir_name << std::endl;
    }
}

std::string FolderNavigation::getBreadcrumbPath() {
    std::string breadcrumb;
    for (const auto& path : breadcrumbTrail) {
        breadcrumb += " > " + path;
    }
    return breadcrumb;
}

void FolderNavigation::goBack() {
    if (breadcrumbTrail.size() > 1) {
        breadcrumbTrail.pop_back();
        currentPath = breadcrumbTrail.back();
        chdir(currentPath.c_str());
    }
}
