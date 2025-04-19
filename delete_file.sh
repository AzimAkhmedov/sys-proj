#!/bin/bash

# Function to prompt user for a correct file name
prompt_for_correct_file() {
    while true; do
        read -p "File not found! Please enter the correct filename or type 'exit' to cancel: " new_file
        if [[ "$new_file" == "exit" ]]; then
            echo "Operation cancelled."
            exit 1
        elif [ -f "$new_file" ]; then
            echo "Deleting file: $new_file"
            rm "$new_file"
            echo "File deleted successfully!"
            exit 0
        else
            echo "Error: File '$new_file' not found. Try again."
        fi
    done
}
# Check if file exists before deleting
if [ -f "$1" ]; then
    rm "$1"
    echo "File deleted: $1"
    exit 0
else
    echo "Error: File '$1' not found!"
    prompt_for_correct_file  # Ask user for correct file name
fi

