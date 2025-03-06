#include <iostream>
#include "file_manager.h"
using namespace std;



void clickOption1(GtkWidget* widget, gpointer data) {
    cout << "Option 1 clicked" << endl;
}

int main() {
    std::vector<Option> options;

    options.push_back(Option(0, "Option 1", clickOption1));
    options.push_back(Option(1, "Option 2", Menu::on_button_clicked));
    options.push_back(Option(2, "Option 3", Menu::on_button_clicked));
    options.push_back(Option(3, "Option 4", Menu::on_button_clicked));
    options.push_back(Option(4, "Option 5", Menu::on_button_clicked));
    options.push_back(Option(5, "Option 6", Menu::on_button_clicked));



    cout << "Hello World" << endl;

    
    
    Menu menu(options);
    
    menu.run();

    return 0;
}