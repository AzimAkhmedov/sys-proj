#include "file_manager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

Explorer::Explorer() {
    getcwd(current_path, sizeof(current_path));

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Explorer");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled);

    list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrolled), list_box);

    updateList();

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void Explorer::on_item_clicked(GtkWidget *widget, gpointer data)
{
    Explorer *explorer = static_cast<Explorer *>(data);
    const char *item_name = gtk_button_get_label(GTK_BUTTON(widget));

    if (strcmp(item_name, "Go Back") == 0)
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

void Explorer::on_delete_clicked(GtkWidget *widget, gpointer data)
{
    auto *pair = static_cast<std::pair<Explorer *, std::string> *>(data);
    Explorer *explorer = pair->first;
    std::string item_name = pair->second;

    std::string full_path = std::string(explorer->current_path) + "/" + item_name;

    // Create confirmation dialog
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(explorer->window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_QUESTION,
                                               GTK_BUTTONS_YES_NO,
                                               "Are you sure you want to delete '%s'?",
                                               item_name.c_str());

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (response == GTK_RESPONSE_YES)
    {
        struct stat path_stat;
        if (stat(full_path.c_str(), &path_stat) == 0)
        {
            bool success = false;
            if (S_ISDIR(path_stat.st_mode))
            {
                // For simplicity, we'll only delete empty directories
                success = (rmdir(full_path.c_str()) == 0);
            }
            else if (S_ISREG(path_stat.st_mode))
            {
                success = (unlink(full_path.c_str()) == 0);
            }

            if (!success)
            {
                GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(explorer->window),
                                                                 GTK_DIALOG_MODAL,
                                                                 GTK_MESSAGE_ERROR,
                                                                 GTK_BUTTONS_OK,
                                                                 "Failed to delete '%s'",
                                                                 item_name.c_str());
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }
            else
            {
                explorer->updateList(); // Refresh the list after successful deletion
            }
        }
    }

    delete pair; // Clean up the allocated pair
}

bool isImage(const std::string& filename) {
    const char* exts[] = {".png", ".jpg", ".jpeg", ".gif", ".bmp", ".webp"};
    for (const auto& ext : exts) {
        if (filename.size() >= strlen(ext) && filename.compare(filename.size() - strlen(ext), strlen(ext), ext) == 0)
            return true;
    }
    return false;
}

bool isAudio(const std::string& filename) {
    const char* exts[] = {".mp3", ".wav", ".aac", ".ogg", ".flac"};
    for (const auto& ext : exts) {
        if (filename.size() >= strlen(ext) && filename.compare(filename.size() - strlen(ext), strlen(ext), ext) == 0)
            return true;
    }
    return false;
}

bool isVideo(const std::string& filename) {
    const char* exts[] = {".mp4", ".avi", ".mov", ".mkv", ".webm"};
    for (const auto& ext : exts) {
        if (filename.size() >= strlen(ext) && filename.compare(filename.size() - strlen(ext), strlen(ext), ext) == 0)
            return true;
    }
    return false;
}

bool isDocument(const std::string& filename) {
    const char* exts[] = {".pdf", ".doc", ".docx", ".txt", ".xls", ".xlsx", ".ppt", ".pptx"};
    for (const auto& ext : exts) {
        if (filename.size() >= strlen(ext) && filename.compare(filename.size() - strlen(ext), strlen(ext), ext) == 0)
            return true;
    }
    return false;
}

void Explorer::updateList() {
    DIR *dir = opendir(current_path);
    if (!dir) {
        std::cerr << "Failed to open directory: " << current_path << std::endl;
        return;
    }

    gtk_container_foreach(GTK_CONTAINER(list_box), (GtkCallback)gtk_widget_destroy, NULL);

    std::vector<std::pair<std::string, std::string>> images, audios, videos, documents;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0)
            continue;

        std::string full_path = std::string(current_path) + "/" + entry->d_name;
        struct stat path_stat;
        if (stat(full_path.c_str(), &path_stat) != 0) continue;

        if (S_ISREG(path_stat.st_mode)) {
            if (isImage(entry->d_name)) images.emplace_back(entry->d_name, full_path);
            else if (isAudio(entry->d_name)) audios.emplace_back(entry->d_name, full_path);
            else if (isVideo(entry->d_name)) videos.emplace_back(entry->d_name, full_path);
            else if (isDocument(entry->d_name)) documents.emplace_back(entry->d_name, full_path);
        }
    }
    closedir(dir);

    auto addSection = [this](const char* title, const std::vector<std::pair<std::string, std::string>>& items) {
        if (items.empty()) return;

        GtkWidget* section_label = gtk_label_new(title);
        gtk_widget_set_name(section_label, "section-label");
        gtk_box_pack_start(GTK_BOX(list_box), section_label, FALSE, FALSE, 5);

        for (const auto& item : items) {
            GtkWidget *item_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
            GtkWidget *icon = gtk_image_new_from_icon_name("text-x-generic", GTK_ICON_SIZE_BUTTON);
            gtk_box_pack_start(GTK_BOX(item_box), icon, FALSE, FALSE, 0);

            GtkWidget *item_button = gtk_button_new_with_label(item.first.c_str());
            gtk_button_set_relief(GTK_BUTTON(item_button), GTK_RELIEF_NONE);
            gtk_box_pack_start(GTK_BOX(item_box), item_button, TRUE, TRUE, 0);
            g_signal_connect(item_button, "clicked", G_CALLBACK(on_item_clicked), this);

            GtkWidget *delete_button = gtk_button_new_with_label("Delete");
            auto *data = new std::pair<Explorer*, std::string>(this, item.first);
            g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_clicked), data);
            gtk_box_pack_start(GTK_BOX(item_box), delete_button, FALSE, FALSE, 0);

            gtk_container_add(GTK_CONTAINER(list_box), item_box);
        }
    };

    addSection("Images", images);
    addSection("Audios", audios);
    addSection("Videos", videos);
    addSection("Documents", documents);

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
    gtk_window_set_title(GTK_WINDOW(window), "Modern File Explorer");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Load CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Create main container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Create scrolled window for list
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled_window, TRUE, TRUE, 0);

    // Create list box
    list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), list_box);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    updateList();
    gtk_widget_show_all(window);

    gtk_main();
}
