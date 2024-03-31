#!/bin/bash
if [ $# -ne 2 ]; then
    echo 'parameters are not specified'
    exit 1
fi

if [ ! -e $1 ]; then
    echo "$1 path does not exist"
    exit 1
elif [ ! -d $1 ]
then
    echo "$1 is not a directory"
    exit 1
fi

# Initialize counters
file_count=0
match_count=0

# Use 'find' to locate files in the directory and subdirectories
while IFS= read -r -d '' file; do
  # Increment file count
  ((file_count++))
  
  # Counting matching lines in the file using 'grep'
  match_lines=$(grep -c "$2" "$file")
  
  # If there are matching lines, increment match count
  match_count=$((match_count+match_lines))
done < <(find "$1" -type f -print0)

echo "The number of files are $file_count and the number of matching lines are $match_count"