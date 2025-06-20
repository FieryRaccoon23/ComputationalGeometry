#!/bin/bash

# Destination directory
DEST="/mnt/d/ComputationalGeometry/mesh"

# Check if at least one file is provided
if [ $# -lt 1 ]; then
  echo "Usage: $0 <file1> <file2> ... <fileN>"
  exit 1
fi

# Loop through all arguments
for file in "$@"
do
  if [ -f "$file" ]; then
    echo "Copying $file to $DEST"
    cp "$file" "$DEST"
  else
    echo "Warning: '$file' does not exist or is not a regular file"
  fi
done

echo "Done."
