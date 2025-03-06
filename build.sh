#!/bin/bash

g++ -std=c++11 menu.cpp main.cpp -o menu `pkg-config --cflags --libs gtk+-3.0`

if [ $? -ne 0 ]; then
    echo "Error occurred while compiling the code."
fi
