#include <iostream>
#include "file_manager.h"
using namespace std;

void handleOpenFolderNavigation(GtkWidget *widget, gpointer data)
{
    Menu *menu = static_cast<Menu *>(data);
    menu->closeMenu();

    Explorer explorer;
    explorer.run();
}

void clickOption1(GtkWidget *widget, gpointer data)
{
    cout << "Option 1 clicked" << endl;
}

void runMenu()
{

    std::vector<Option> options;

    options.push_back(Option(0, "Navigation", handleOpenFolderNavigation));
    options.push_back(Option(1, "Write New File", Menu::on_button_clicked));
    options.push_back(Option(2, "", Menu::on_button_clicked));
    options.push_back(Option(3, "Option 4", Menu::on_button_clicked));
    options.push_back(Option(4, "Option 5", Menu::on_button_clicked));
    options.push_back(Option(5, "Option 6", Menu::on_button_clicked));

    Menu menu(options);

    menu.run();
}

int main(int argc, char *argv[])

{
    App appState;
    gtk_init(&argc, &argv);
    cout << appState.getAppState() << endl;
    runMenu();

    return 0;
}