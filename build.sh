#!/bin/bash

# Compile with GTK+3 and additional dependencies for icons and styling
g++ -std=c++11 menu.cpp explorer.cpp main.cpp file_manager.cpp -D_GLIBCXX_USE_CXX11_ABI=0 -o menu \
    `pkg-config --cflags --libs gtk+-3.0` \
    `pkg-config --cflags --libs gdk-3.0` \
    `pkg-config --cflags --libs gio-2.0`

if [ $? -ne 0 ]; then
    echo "Error occurred while compiling the code."
fi
