#!/bin/bash

# Check if a folder path is provided
if [ -z "$1" ]; then
  echo "Error: Please provide a folder path as an argument."
  exit 1
fi

# Get the folder name
folder_name=$(basename "$1")

# Create a ZIP archive of the folder
zip -r "$folder_name.zip" "$1"