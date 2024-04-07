#!/bin/sh
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

file_count=$(find "$1" -type f | wc -l)
match_count=$(grep -r "$2" "$1" | wc -l)

echo "The number of files are $file_count and the number of matching lines are $match_count"