#!/bin/bash

SOURCE_FOLDER=${1:-src}  # default to 'src' if not passed
OUTPUT_FOLDER="build"

echo "🔧 Running CMake generation..."
cmake -DCMAKE_BUILD_TYPE=Release -DSOURCE_FOLDER="$SOURCE_FOLDER" -DOUTPUT_FOLDER="$OUTPUT_FOLDER" -B "$OUTPUT_FOLDER"

LINK_FILE="$OUTPUT_FOLDER/CMakeFiles/app.dir/link.txt"

if [[ -f "$LINK_FILE" ]]; then
  echo "🧹 Removing -lFALSE from $LINK_FILE"
  sed -i 's/-lFALSE//g' "$LINK_FILE"
else
  echo "⚠️ $LINK_FILE not found — skipping -lFALSE cleanup"
fi

echo "✅ CMake setup complete. You can now run: ./make.sh"