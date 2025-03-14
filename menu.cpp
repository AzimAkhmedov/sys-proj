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