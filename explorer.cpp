#include "file_manager.h"
#include "folder_navigation.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>

class Explorer {
    private:
        GtkWidget *window;
        GtkWidget *list_box;
        char current_path[1024];
        FolderNavigation advancedNavigator; // New breadcrumb-based navigation
    
        void updateList();
        void navigateTo(const char *dir_name);
        void previewFile(const char *file_path);
        static void on_delete_clicked(GtkWidget *widget, gpointer data);

    
    public:
        Explorer();
        void run();
        static void on_item_clicked(GtkWidget *widget, gpointer data);
    };
    
Explorer::Explorer()
{
    getcwd(current_path, sizeof(current_path)); // Get current working directory
}

void Explorer::on_item_clicked(GtkWidget *widget, gpointer data)
{
    Explorer *explorer = static_cast<Explorer *>(data);
    const char *item_name = gtk_button_get_label(GTK_BUTTON(widget));

    if (strcmp(item_name, "Go Back") == 0) // Handle "Go Back" click
    {
        explorer->navigateTo("..");
        return;
    }

    std::string full_path = std::string(explorer->current_path) + "/" + item_name;

    struct stat path_stat;
    if (stat(full_path.c_str(), &path_stat) == 0)
    {
        if (S_ISDIR(path_stat.st_mode))
        {
            explorer->navigateTo(item_name);
        }
        else if (S_ISREG(path_stat.st_mode))
        {
            explorer->previewFile(full_path.c_str());
        }
    }
}

void Explorer::updateList() {
    // Clear old list before updating
    gtk_container_foreach(GTK_CONTAINER(list_box), (GtkCallback)gtk_widget_destroy, NULL);

    // Add breadcrumb label to show current path
    GtkWidget *breadcrumb_label = gtk_label_new(advancedNavigator.getBreadcrumbPath().c_str());
    gtk_container_add(GTK_CONTAINER(list_box), breadcrumb_label);

    // Add "Go Back" button if not in root directory
    if (advancedNavigator.getBreadcrumbPath() != "/") {
        GtkWidget *back_button = gtk_button_new_with_label("Go Back");
        g_signal_connect(back_button, "clicked", G_CALLBACK(on_item_clicked), this);
        gtk_container_add(GTK_CONTAINER(list_box), back_button);
    }

    // Open the current directory
    DIR *dir = opendir(advancedNavigator.getCurrentPath().c_str());
    if (!dir) {
        std::cerr << "Failed to open directory: " << advancedNavigator.getCurrentPath() << std::endl;
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0) continue; // Ignore current folder

        std::string fullPath = advancedNavigator.getCurrentPath() + "/" + entry->d_name;

        // Create a horizontal box (file/folder + delete button if it's a file)
        GtkWidget *fileBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

        // Create a button for the file/folder name
        GtkWidget *fileButton = gtk_button_new_with_label(entry->d_name);
        g_signal_connect(fileButton, "clicked", G_CALLBACK(on_item_clicked), this);
        gtk_box_pack_start(GTK_BOX(fileBox), fileButton, TRUE, TRUE, 0);

        // Check if it's a file (not a folder)
        struct stat path_stat;
        stat(fullPath.c_str(), &path_stat);

        if (S_ISREG(path_stat.st_mode)) {  // If it's a file, add a delete button
            GtkWidget *deleteButton = gtk_button_new_with_label("Delete");
            g_signal_connect(deleteButton, "clicked", G_CALLBACK(on_delete_clicked), this);
            gtk_box_pack_start(GTK_BOX(fileBox), deleteButton, FALSE, FALSE, 0);
        }

        // Add the fileBox (file/folder + delete button) to the list
        gtk_container_add(GTK_CONTAINER(list_box), fileBox);
    }

    closedir(dir);
    gtk_widget_show_all(list_box);  // Refresh UI to show updated file list
}

void Explorer::navigateTo(const char *dir_name) {
    // Use the new navigation system
    advancedNavigator.navigateTo(dir_name);

    // Update the UI
    updateList();
}

void Explorer::previewFile(const char *file_path)
{
    std::ifstream file(file_path);
    if (!file)
    {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    GtkWidget *dialog = gtk_dialog_new_with_buttons("File Preview", GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL, "_OK", GTK_RESPONSE_OK, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(content.c_str());

    gtk_label_set_selectable(GTK_LABEL(label), TRUE); // Allow text selection
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

void Explorer::run()
{
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Explorer");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), list_box);

    updateList();
    gtk_widget_show_all(window);

    gtk_main();
}
void Explorer::on_delete_clicked(GtkWidget *widget, gpointer data) {
    Explorer *explorer = static_cast<Explorer *>(data);
    const char *file_name = gtk_button_get_label(GTK_BUTTON(widget));
    std::string full_path = std::string(explorer->current_path) + "/" + file_name;

    // Confirm before deleting
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(explorer->window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_WARNING,
                                               GTK_BUTTONS_YES_NO,
                                               "Are you sure you want to delete '%s'?", file_name);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); // Always destroy dialog

    if (response == GTK_RESPONSE_YES) {
        if (deleteFile(full_path)) {
            explorer->updateList(); // Refresh the list after deletion
        } else {
            std::cerr << "Failed to delete file: " << full_path << std::endl;
        }
    }
}
