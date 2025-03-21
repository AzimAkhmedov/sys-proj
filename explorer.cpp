#include "file_manager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>

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

void Explorer::updateList()
{
    DIR *dir = opendir(current_path);
    if (!dir)
    {
        std::cerr << "Failed to open directory: " << current_path << std::endl;
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(list_box), (GtkCallback)gtk_widget_destroy, NULL); // Clear list

    // Add "Go Back" button to go up one directory
    if (strcmp(current_path, "/") != 0) // Only show if not already in root
    {
        GtkWidget *back_button = gtk_button_new_with_label("Go Back");
        g_signal_connect(back_button, "clicked", G_CALLBACK(on_item_clicked), this);
        gtk_container_add(GTK_CONTAINER(list_box), back_button);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0) // Ignore current directory "."
            continue;

        GtkWidget *button = gtk_button_new_with_label(entry->d_name);
        g_signal_connect(button, "clicked", G_CALLBACK(on_item_clicked), this);
        gtk_container_add(GTK_CONTAINER(list_box), button);
    }

    closedir(dir);
    gtk_widget_show_all(list_box);
}

void Explorer::navigateTo(const char *dir_name)
{
    if (strcmp(dir_name, "..") == 0)
    {
        chdir(".."); // Move to parent directory
    }
    else
    {
        std::string new_path = std::string(current_path) + "/" + dir_name;
        if (chdir(new_path.c_str()) == 0) // Change directory
        {
            getcwd(current_path, sizeof(current_path));
        }
    }

    updateList(); // Refresh list
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