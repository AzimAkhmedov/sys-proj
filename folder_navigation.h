#ifndef FOLDER_NAVIGATION_H
#define FOLDER_NAVIGATION_H

#include <string>
#include <vector>

class FolderNavigation {
private:
    std::vector<std::string> breadcrumbTrail;
    std::string currentPath;

public:
    FolderNavigation();  
    void navigateTo(const std::string& dir_name);
    std::string getBreadcrumbPath();
    void goBack();
};

#endif // FOLDER_NAVIGATION_H
