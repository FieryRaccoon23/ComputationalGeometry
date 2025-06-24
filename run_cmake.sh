#!/bin/bash

BUILD_MODE=${1:-training}            # training (default) or inference
BUILD_TYPE_INPUT=${2:-release}       # release (default) or debug

# Normalize inputs
BUILD_MODE=$(echo "$BUILD_MODE" | tr '[:upper:]' '[:lower:]')
BUILD_TYPE_INPUT=$(echo "$BUILD_TYPE_INPUT" | tr '[:upper:]' '[:lower:]')

# Determine CMake build type and output folder
if [[ "$BUILD_TYPE_INPUT" == "debug" ]]; then
  CMAKE_BUILD_TYPE="Debug"
  OUTPUT_FOLDER="buildDebug_${BUILD_MODE}"
else
  CMAKE_BUILD_TYPE="Release"
  OUTPUT_FOLDER="build_${BUILD_MODE}"
fi

echo "🔧 Running CMake generation..."
echo "👉 BUILD_MODE (SOURCE_FOLDER): $BUILD_MODE"
echo "👉 BUILD_TYPE: $CMAKE_BUILD_TYPE"
echo "👉 OUTPUT_FOLDER: $OUTPUT_FOLDER"

cmake -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -DSOURCE_FOLDER="$BUILD_MODE" -DOUTPUT_FOLDER="$OUTPUT_FOLDER" -B "$OUTPUT_FOLDER"

LINK_FILE="$OUTPUT_FOLDER/CMakeFiles/app.dir/link.txt"

if [[ -f "$LINK_FILE" ]]; then
  echo "🧹 Removing -lFALSE from $LINK_FILE"
  sed -i 's/-lFALSE//g' "$LINK_FILE"
else
  echo "⚠️ $LINK_FILE not found — skipping -lFALSE cleanup"
fi

echo "✅ CMake setup complete."