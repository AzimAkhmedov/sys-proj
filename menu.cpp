#include "file_manager.h"
#include <iostream>
#include <thread>

GtkWidget *content_view;
GtkWidget *sidebar_list;
std::string current_folder;

struct DirectoryStats
{
    int num_files = 0;
    int num_dirs = 0;
    uintmax_t total_size = 0;
};

DirectoryStats get_directory_stats(const std::string &path)
{
    DirectoryStats stats;

    for (const auto &entry : fs::directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            stats.num_files++;
            stats.total_size += entry.file_size();
        }
        else if (entry.is_directory())
        {
            stats.num_dirs++;
        }
    }

    return stats;
}

extern "C" void on_file_clicked(GtkWidget *widget, gpointer data)
{
    if (data)
    {
        std::string *file_path = static_cast<std::string *>(data);
        display_file_content(*file_path);
    }
}

extern "C" void on_file_button_clicked(GtkWidget *widget, gpointer data)
{
    if (data)
    {
        std::string *file_path = static_cast<std::string *>(data);
        display_file_content(*file_path);
        delete file_path; // Prevent memory leak
    }
}
std::map<std::string, std::vector<std::string>> categorize_files(const std::string &folder)
{
    std::map<std::string, std::vector<std::string>> categories = {
        {"Images", {}}, {"Music", {}}, {"Documents", {}}, {"Others", {}}};

    for (const auto &entry : fs::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
            continue;

        std::string ext = entry.path().extension();
        std::string path = entry.path().string();

        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".gif")
        {
            categories["Images"].push_back(path);
        }
        else if (ext == ".mp3" || ext == ".wav" || ext == ".flac")
        {
            categories["Music"].push_back(path);
        }
        else if (ext == ".txt" || ext == ".pdf" || ext == ".docx")
        {
            categories["Documents"].push_back(path);
        }
        else
        {
            categories["Others"].push_back(path);
        }
    }
    return categories;
}

void update_tag_sidebar(GtkWidget *tag_sidebar, const std::string &file_path)
{
    gtk_container_foreach(GTK_CONTAINER(tag_sidebar), (GtkCallback)gtk_widget_destroy, NULL);

    std::vector<std::string> tags = {"Important", "To Review", "Archive", "Work", "Personal"};

    GtkWidget *label = gtk_label_new("Tags");
    gtk_container_add(GTK_CONTAINER(tag_sidebar), label);

    for (const std::string &tag : tags)
    {
        GtkWidget *tag_button = gtk_button_new_with_label(tag.c_str());
        gtk_container_add(GTK_CONTAINER(tag_sidebar), tag_button);
    }

    gtk_widget_show_all(tag_sidebar);
}

void update_sidebar(const std::string &folder)
{
    current_folder = folder;
    auto categorized_files = categorize_files(folder);

    gtk_container_foreach(GTK_CONTAINER(sidebar_list), (GtkCallback)gtk_widget_destroy, NULL);

    for (const auto &[category, files] : categorized_files)
    {
        if (files.empty())
            continue;

        GtkWidget *category_label = gtk_label_new(category.c_str());
        gtk_container_add(GTK_CONTAINER(sidebar_list), category_label);

        for (const std::string &file : files)
        {
            GtkWidget *file_button = gtk_button_new_with_label(fs::path(file).filename().c_str());
            g_signal_connect(file_button, "clicked", G_CALLBACK(on_file_button_clicked), new std::string(file));
            gtk_container_add(GTK_CONTAINER(sidebar_list), file_button);
        }
    }
    gtk_widget_show_all(sidebar_list);
}

void display_file_content(const std::string &file_path)
{
    std::ifstream file(file_path);
    std::string content, line;

    if (file)
    {
        while (getline(file, line))
        {
            content += line + "\n";
        }
        file.close();
    }
    else
    {
        content = "Cannot open file: " + file_path;
    }

    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(content_view)),
        content.c_str(),
        -1);
}

Menu::Menu(const std::vector<Option> &options) : menuOptions(options)
{
    gtk_init(nullptr, nullptr);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Menu");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    for (const auto &option : menuOptions)
    {
        GtkWidget *button = gtk_button_new_with_label(option.label.c_str());
        g_signal_connect(button, "clicked", G_CALLBACK(option.callback), this);
        gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    }

    // Exit button
    GtkWidget *exitButton = gtk_button_new_with_label("Exit");
    g_signal_connect(exitButton, "clicked", G_CALLBACK(&Menu::on_exit_clicked), nullptr);
    gtk_box_pack_start(GTK_BOX(vbox), exitButton, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

void Menu::run()
{
    gtk_main();
}

void Menu::on_button_clicked(GtkWidget *widget, gpointer data)
{
    std::cout << "Selected: " << static_cast<const char *>(data) << std::endl;
}

void Menu::on_exit_clicked(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

void Menu::closeMenu()
{
    gtk_main_quit();
}

void showMessageDialog(const std::string &message, GtkMessageType type)
{
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                               GTK_DIALOG_MODAL,
                                               type,
                                               GTK_BUTTONS_OK,
                                               "%s", message.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void Menu::onSearchClicked(GtkWidget *widget, gpointer data)
{
    Menu *this_ptr = static_cast<Menu *>(data);

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Search Files",
                                                    GTK_WINDOW(this_ptr->window),
                                                    GTK_DIALOG_MODAL,
                                                    "_Search", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter file name or part...");
    gtk_box_pack_start(GTK_BOX(content_area), entry, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
    {
        const gchar *query = gtk_entry_get_text(GTK_ENTRY(entry));
        std::string search_query = query ? query : "";

        if (!search_query.empty())
        {
            // Запускаем поиск в фоновом потоке
            std::thread([this_ptr, search_query]()
                        {
                std::vector<std::string> results;
                std::error_code ec;

                for (fs::recursive_directory_iterator it("/", fs::directory_options::skip_permission_denied, ec), end;
                     it != end; it.increment(ec))
                {
                    const auto &entry = *it;
                    if (entry.is_regular_file(ec))
                    {
                        std::string name = entry.path().filename().string();
                        if (name.find(search_query) != std::string::npos)
                        {
                            results.push_back(entry.path().string());
                        }
                    }
                }

                // Создаём окно с результатами в основном потоке
                g_idle_add([](gpointer user_data) -> gboolean {
                    auto data = static_cast<std::pair<Menu *, std::vector<std::string>> *>(user_data);
                    Menu *this_ptr = data->first;
                    const std::vector<std::string> &results = data->second;

                    GtkWidget *resultWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
                    gtk_window_set_title(GTK_WINDOW(resultWindow), "Search Results");
                    gtk_window_set_default_size(GTK_WINDOW(resultWindow), 500, 400);

                    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
                    GtkWidget *resultBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
                    gtk_container_add(GTK_CONTAINER(scroll), resultBox);
                    gtk_container_add(GTK_CONTAINER(resultWindow), scroll);

                    if (results.empty())
                    {
                        GtkWidget *label = gtk_label_new("No files found.");
                        gtk_container_add(GTK_CONTAINER(resultBox), label);
                    }
                    else
                    {
                        for (const std::string &path : results)
                        {
                            GtkWidget *btn = gtk_button_new_with_label(fs::path(path).filename().c_str());
                            g_signal_connect(btn, "clicked", G_CALLBACK(on_file_button_clicked), new std::string(path));
                            gtk_box_pack_start(GTK_BOX(resultBox), btn, FALSE, FALSE, 0);
                        }
                    }

                    gtk_widget_show_all(resultWindow);
                    delete data;
                    return FALSE;
                }, new std::pair<Menu *, std::vector<std::string>>(this_ptr, results)); })
                .detach(); // Фоновые потоки не блокируют интерфейс
        }
    }

    gtk_widget_destroy(dialog);
}
void Menu::onExplorerClicked(GtkWidget *widget, gpointer data)
{
    // Приводим data к Menu*, чтобы получить доступ к членам класса
    Menu *this_ptr = static_cast<Menu *>(data);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select a File",
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        std::string file_path(filename);
        g_free(filename);

        // Окно для проводника
        this_ptr->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(this_ptr->window), "File Explorer");
        gtk_window_set_default_size(GTK_WINDOW(this_ptr->window), 800, 600);

        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_container_add(GTK_CONTAINER(this_ptr->window), hbox);

        // Боковая панель
        sidebar_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        GtkWidget *sidebar_scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_widget_set_size_request(sidebar_scroll, 200, -1);
        gtk_container_add(GTK_CONTAINER(sidebar_scroll), sidebar_list);
        gtk_box_pack_start(GTK_BOX(hbox), sidebar_scroll, FALSE, FALSE, 0);

        // Вид содержимого файла
        content_view = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(content_view), FALSE);
        GtkWidget *content_scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_container_add(GTK_CONTAINER(content_scroll), content_view);
        gtk_box_pack_start(GTK_BOX(hbox), content_scroll, TRUE, TRUE, 0);

        // Обновление боковой панели
        update_sidebar(fs::path(file_path).parent_path().string());

        // Кнопка выбранного файла
        GtkWidget *file_button = gtk_button_new_with_label(fs::path(file_path).filename().c_str());
        g_signal_connect(file_button, "clicked", G_CALLBACK(on_file_clicked), new std::string(file_path));
        gtk_container_add(GTK_CONTAINER(sidebar_list), file_button);

        gtk_widget_show_all(this_ptr->window);

        // Отображение содержимого
        display_file_content(file_path);
    }

    gtk_widget_destroy(dialog);
}
void Menu::onCreateZipClicked(GtkWidget *widget, gpointer data)
{
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

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *folderPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (folderPath)
        {
            std::cout << "Selected folder: " << folderPath << std::endl;
            bool success = createZipFromFolder(folderPath);
            if (success)
            {
                message = "ZIP file created successfully!";
                msgType = GTK_MESSAGE_INFO;
            }
            else
            {
                message = "Failed to create ZIP file.";
                msgType = GTK_MESSAGE_ERROR;
            }
            g_free(folderPath);
        }
        else
        {
            message = "No folder selected!";
            msgType = GTK_MESSAGE_WARNING;
        }
    }
    else
    {
        message = "Operation cancelled.";
        msgType = GTK_MESSAGE_WARNING;
    }

    gtk_widget_destroy(dialog);
    showMessageDialog(message, msgType);
}

void Menu::onCopyFolderClicked(GtkWidget *widget, gpointer data)
{
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

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *folderPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (folderPath)
        {
            std::cout << "Selected folder: " << folderPath << std::endl;
            bool success = copyFolder(folderPath);
            if (success)
            {
                message = "Folder copied successfully!";
                msgType = GTK_MESSAGE_INFO;
            }
            else
            {
                message = "Failed to copy folder.";
                msgType = GTK_MESSAGE_ERROR;
            }
            g_free(folderPath);
        }
        else
        {
            message = "No folder selected!";
            msgType = GTK_MESSAGE_WARNING;
        }
    }
    else
    {
        message = "Operation cancelled.";
        msgType = GTK_MESSAGE_WARNING;
    }

    gtk_widget_destroy(dialog);
    showMessageDialog(message, msgType);
}
void Menu::onDeleteFileClicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    dialog = gtk_file_chooser_dialog_new("Select File to Delete",
                                         NULL,
                                         action,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Delete", GTK_RESPONSE_ACCEPT,
                                         NULL);

    std::string message;
    GtkMessageType msgType = GTK_MESSAGE_INFO;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (filePath)
        {
            std::cout << "Selected file: " << filePath << std::endl;

            // Confirm before deleting
            GtkWidget *confirmDialog = gtk_message_dialog_new(NULL,
                                                              GTK_DIALOG_MODAL,
                                                              GTK_MESSAGE_WARNING,
                                                              GTK_BUTTONS_YES_NO,
                                                              "Are you sure you want to delete '%s'?", filePath);
            int response = gtk_dialog_run(GTK_DIALOG(confirmDialog));
            gtk_widget_destroy(confirmDialog);

            if (response == GTK_RESPONSE_YES)
            {
                bool success = deleteFile(filePath);
                if (success)
                {
                    message = "File deleted successfully!";
                    msgType = GTK_MESSAGE_INFO;
                }
                else
                {
                    message = "Failed to delete file.";
                    msgType = GTK_MESSAGE_ERROR;
                }
            }
            else
            {
                message = "File deletion cancelled.";
                msgType = GTK_MESSAGE_WARNING;
            }

            g_free(filePath);
        }
        else
        {
            message = "No file selected!";
            msgType = GTK_MESSAGE_WARNING;
        }
    }
    else
    {
        message = "Operation cancelled.";
        msgType = GTK_MESSAGE_WARNING;
    }

    gtk_widget_destroy(dialog);
    showMessageDialog(message, msgType);
}
