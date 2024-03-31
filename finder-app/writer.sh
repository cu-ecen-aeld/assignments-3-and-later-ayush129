#!/bin/bash
if [ $# -ne 2 ]; then
    echo 'parameters are not specified'
    exit 1
fi

file_path="$1"
content="$2"

# Extract the directory path from the file_path
directory_path="$(dirname "$file_path")"

# Create the directory if it doesn't exist
if [ ! -d "$directory_path" ]; then
  if ! mkdir -p "$directory_path"; then
    echo "Error: Failed to create directory: $directory_path"
    exit 1
  fi
fi

# Write the content to the file, overwriting if it exists
if ! echo "$content" > "$file_path"; then
  echo "Error: Failed to create or write to file: $file_path"
  exit 1
fi