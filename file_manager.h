#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <gtk/gtk.h>
#include <vector>
#include <string>

#include <string>

class App
{
private:
    //  "menu" | "navigation" | "write" | "read" | "exit"
    std::string appState;

public:
    void setAppState(std::string state)
    {
        appState = state;
    }

    std::string getAppState()
    {

        return appState;
    }
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

#endif // FILE_MANAGER
