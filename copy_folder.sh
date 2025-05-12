#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: Please provide a folder path as an argument."
    exit 1
fi

if [ ! -d "$1" ]; then
    echo "Error: The provided path is not a valid directory."
    exit 1
fi

folder_name=$(basename "$1")
destination="Copied_Folder_$folder_name"

cp -R "$1" "$destination"

echo "Folder copied successfully to: $destination"
