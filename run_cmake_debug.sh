#!/bin/bash

SOURCE_FOLDER=${1:-src}  # default to 'src' if not passed
OUTPUT_FOLDER="buildDebug"

echo "🔧 Running CMake generation for Debug..."
cmake -DCGAL_DIR=$PWD/libs/CGAL-6.0.1 -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=$PWD/libs/boost_1_88_0 -DSOURCE_FOLDER="$SOURCE_FOLDER" -DOUTPUT_FOLDER="$OUTPUT_FOLDER" -B "$OUTPUT_FOLDER"

LINK_FILE="$OUTPUT_FOLDER/CMakeFiles/app.dir/link.txt"

if [[ -f "$LINK_FILE" ]]; then
  echo "🧹 Removing -lFALSE from $LINK_FILE"
  sed -i 's/-lFALSE//g' "$LINK_FILE"
else
  echo "⚠️ $LINK_FILE not found — skipping -lFALSE cleanup"
fi

echo "✅ CMake setup complete. You can now run: make -C $OUTPUT_FOLDER"