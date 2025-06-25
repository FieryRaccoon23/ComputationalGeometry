#!/bin/bash

FOLDER=${1:-}                                      # e.g., DelaunayTriangulation
BUILD_MODE=${2:-training}                          # training (default) or inference
BUILD_TYPE_INPUT=${3:-release}                     # release (default) or debug

# Normalize inputs
BUILD_MODE=$(echo "$BUILD_MODE" | tr '[:upper:]' '[:lower:]')
BUILD_TYPE_INPUT=$(echo "$BUILD_TYPE_INPUT" | tr '[:upper:]' '[:lower:]')

# Determine CMake build type
if [[ "$BUILD_TYPE_INPUT" == "debug" ]]; then
  CMAKE_BUILD_TYPE="Debug"
  BASE_OUTPUT_FOLDER="buildDebug_${BUILD_MODE}"
else
  CMAKE_BUILD_TYPE="Release"
  BASE_OUTPUT_FOLDER="build_${BUILD_MODE}"
fi

# Compose source folder
if [[ -n "$FOLDER" ]]; then
  SOURCE_FOLDER="$BUILD_MODE/$FOLDER"
  OUTPUT_FOLDER="$BASE_OUTPUT_FOLDER/$FOLDER"
else
  SOURCE_FOLDER="$BUILD_MODE"
  OUTPUT_FOLDER="$BASE_OUTPUT_FOLDER"
fi

echo "🔧 Running CMake generation..."
echo "👉 BUILD_MODE: $BUILD_MODE"
echo "👉 FOLDER: $FOLDER"
echo "👉 SOURCE_FOLDER: $SOURCE_FOLDER"
echo "👉 BUILD_TYPE: $CMAKE_BUILD_TYPE"
echo "👉 OUTPUT_FOLDER: $OUTPUT_FOLDER"

cmake -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -DSOURCE_FOLDER="$SOURCE_FOLDER" -DOUTPUT_FOLDER="$OUTPUT_FOLDER" -B "$OUTPUT_FOLDER"

LINK_FILE="$OUTPUT_FOLDER/CMakeFiles/app.dir/link.txt"

if [[ -f "$LINK_FILE" ]]; then
  echo "🧹 Removing -lFALSE from $LINK_FILE"
  sed -i 's/-lFALSE//g' "$LINK_FILE"
else
  echo "⚠️ $LINK_FILE not found — skipping -lFALSE cleanup"
fi

echo "✅ CMake setup complete."
