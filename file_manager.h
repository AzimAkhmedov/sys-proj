#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>

#include <unistd.h>

namespace fs = std::__fs::filesystem; // For MAC 


void display_file_content(const std::string& file_path);

class App
{
private:
    std::string appState; // "menu" | "navigation" | "write" | "read" | "exit"

public:
    void setAppState(std::string state) { appState = state; }
    std::string getAppState() { return appState; }
};

class Option
{
public:
    int index;
    std::string label;
    void (*callback)(GtkWidget *, gpointer);

    Option(int index, const std::string &label, void (*callback)(GtkWidget *, gpointer))
        : index(index), label(label), callback(callback) {}
};

class Menu
{
public:
    Menu(const std::vector<Option> &options);
    void run();
    void closeMenu();
    static void on_button_clicked(GtkWidget *widget, gpointer data);
    static void on_exit_clicked(GtkWidget *widget, gpointer data);
    static void onCreateZipClicked(GtkWidget* widget, gpointer data);
    static void onCopyFolderClicked(GtkWidget * widget, gpointer data);
    static void onExplorerClicked(GtkWidget *widget, gpointer data);
    static void onDeleteFileClicked(GtkWidget *widget, gpointer data);


private:
    GtkWidget *window;
    GtkWidget *vbox;
    std::vector<Option> menuOptions;
};
bool deleteFile(const std::string& filePath);
bool createZipFromFolder(const std::string& folderPath);
bool copyFolder(const std::string& folderPath);

class Explorer
{
private:
    GtkWidget *window;
    GtkWidget *list_box;
    char current_path[1024];

    void updateList();
    void navigateTo(const char *dir_name);
    void previewFile(const char *file_path);

public:
    Explorer();
    void run();
    static void on_item_clicked(GtkWidget *widget, gpointer data);
};

#endif // FILE_MANAGER
