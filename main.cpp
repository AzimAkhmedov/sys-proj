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

    options.push_back(Option(0, "Navigation", Menu::onExplorerClicked));
    options.push_back(Option(1, "Write New File", Menu::on_button_clicked));
    options.push_back(Option(2, "Create ZIP", Menu::onCreateZipClicked));
    options.push_back(Option(3, "Copy Folder", Menu::onCopyFolderClicked));
    options.push_back(Option(4, "Delete file", Menu::onDeleteFileClicked));    
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