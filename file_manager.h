#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <gtk/gtk.h>
#include <vector>
#include <string>


class Option {
public:
    int index;
    std::string label;
    std::function<void()> onClick;
};

class Menu {
public:
    Menu(const std::vector<Option>& options);
    void run();

private:
    GtkWidget* window;
    GtkWidget* vbox;
    std::vector<Option> menuOptions;

    static void on_button_clicked(GtkWidget* widget, gpointer data);
    static void on_exit_clicked(GtkWidget* widget, gpointer data);
};

#endif // FILE_MANAGER
