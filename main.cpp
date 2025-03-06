#include <iostream>
#include "file_manager.h"
using namespace std;

int main() {
    std::vector<std::string> options = {
        "Option 1",
        "Option 2",
        "Option 3",
        "Option 4",
        "Option 5"
    };


    cout << "Hello World" << endl;

    
    
    Menu menu(options);
    
    menu.run();

    return 0;
}