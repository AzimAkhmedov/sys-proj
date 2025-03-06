#!/bin/bash

folder_name=$(basename "$(pwd)")

zip -r "${folder_name}.zip" .

if [ -z "$1" ]; then
  echo "Please provide a path as an argument."
  exit 1
fi

folder_name=$(basename "$1")

zip -r "$folder_name.zip" "$1"
