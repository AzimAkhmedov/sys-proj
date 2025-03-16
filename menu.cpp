#include "file_manager.h"
#include <iostream>

Menu::Menu(const std::vector<Option>& options) : menuOptions(options) {
    gtk_init(nullptr, nullptr);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Menu");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    for (const auto& option : menuOptions) {
        GtkWidget* button = gtk_button_new_with_label(option.label.c_str());
        g_signal_connect(button, "clicked", G_CALLBACK(option.callback), this);
        gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    }

    // Exit button
    GtkWidget* exitButton = gtk_button_new_with_label("Exit");
    g_signal_connect(exitButton, "clicked", G_CALLBACK(&Menu::on_exit_clicked), nullptr);
    gtk_box_pack_start(GTK_BOX(vbox), exitButton, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

void Menu::run() {
    gtk_main();
}

void Menu::on_button_clicked(GtkWidget* widget, gpointer data) {
    std::cout << "Selected: " << static_cast<const char*>(data) << std::endl;
}

void Menu::on_exit_clicked(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

void Menu::closeMenu() {
    gtk_main_quit();
}

void showMessageDialog(const std::string &message, GtkMessageType type) {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_MODAL,
                                               type,
                                               GTK_BUTTONS_OK,
                                               "%s", message.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void Menu::onCreateZipClicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    dialog = gtk_file_chooser_dialog_new("Select Folder",
                                         NULL,
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    std::string message;
    GtkMessageType msgType = GTK_MESSAGE_INFO;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folderPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (folderPath) {
            std::cout << "Selected folder: " << folderPath << std::endl;
            bool success = createZipFromFolder(folderPath);
            if (success) {
                message = "ZIP file created successfully!";
                msgType = GTK_MESSAGE_INFO;
            } else {
                message = "Failed to create ZIP file.";
                msgType = GTK_MESSAGE_ERROR;
            }
            g_free(folderPath);
        } else {
            message = "No folder selected!";
            msgType = GTK_MESSAGE_WARNING;
        }
    } else {
        message = "Operation cancelled.";
        msgType = GTK_MESSAGE_WARNING;
    }

    gtk_widget_destroy(dialog);
    showMessageDialog(message, msgType);
}

void Menu::onCopyFolderClicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    dialog = gtk_file_chooser_dialog_new("Select Folder",
                                         NULL,
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    std::string message;
    GtkMessageType msgType = GTK_MESSAGE_INFO;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folderPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (folderPath) {
            std::cout << "Selected folder: " << folderPath << std::endl;
            bool success = copyFolder(folderPath);
            if (success) {
                message = "Folder copied successfully!";
                msgType = GTK_MESSAGE_INFO;
            } else {
                message = "Failed to copy folder.";
                msgType = GTK_MESSAGE_ERROR;
            }
            g_free(folderPath);
        } else {
            message = "No folder selected!";
            msgType = GTK_MESSAGE_WARNING;
        }
    } else {
        message = "Operation cancelled.";
        msgType = GTK_MESSAGE_WARNING;
    }

    gtk_widget_destroy(dialog);
    showMessageDialog(message, msgType);
}

