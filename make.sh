#!/bin/bash

BUILD_MODE=${1:-training}            # training (default) or inference
BUILD_TYPE_INPUT=${2:-release}       # release (default) or debug

# Normalize inputs
BUILD_MODE=$(echo "$BUILD_MODE" | tr '[:upper:]' '[:lower:]')
BUILD_TYPE_INPUT=$(echo "$BUILD_TYPE_INPUT" | tr '[:upper:]' '[:lower:]')

# Determine build folder
if [[ "$BUILD_TYPE_INPUT" == "debug" ]]; then
  BUILD_FOLDER="buildDebug_${BUILD_MODE}"
else
  BUILD_FOLDER="build_${BUILD_MODE}"
fi

echo "🏗️  Running make in: $BUILD_FOLDER"

if [[ -d "$BUILD_FOLDER" ]]; then
  make -C "$BUILD_FOLDER"
  echo "✅ Make completed in: $BUILD_FOLDER"
else
  echo "❌ Build folder does not exist: $BUILD_FOLDER"
  echo "⚠️  Please run the generation script first!"
fi
