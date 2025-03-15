#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <unistd.h>

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

private:
    GtkWidget *window;
    GtkWidget *vbox;
    std::vector<Option> menuOptions;
};

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