#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <gtk/gtk.h>
#include <vector>
#include <string>


class Option {
public:
    int index;
    std::string label;
    void (*callback)(GtkWidget*, gpointer);

    Option(int index, const std::string& label, void (*callback)(GtkWidget*, gpointer))
        : index(index), label(label), callback(callback) {}
    
};

class Menu {
public:
    Menu(const std::vector<Option>& options);
    void run();
    static void on_button_clicked(GtkWidget* widget, gpointer data);
    static void on_exit_clicked(GtkWidget* widget, gpointer data);
private:
    GtkWidget* window;
    GtkWidget* vbox;
    std::vector<Option> menuOptions;

    
};

#endif // FILE_MANAGER
