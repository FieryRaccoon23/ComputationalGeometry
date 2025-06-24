#!/bin/bash

BUILD_MODE=${1:-training}            # training (default) or inference
BUILD_TYPE_INPUT=${2:-release}       # release (default) or debug

# Normalize inputs
BUILD_MODE=$(echo "$BUILD_MODE" | tr '[:upper:]' '[:lower:]')
BUILD_TYPE_INPUT=$(echo "$BUILD_TYPE_INPUT" | tr '[:upper:]' '[:lower:]')

# Determine folder to remove
if [[ "$BUILD_TYPE_INPUT" == "debug" ]]; then
  TARGET_FOLDER="buildDebug_${BUILD_MODE}"
else
  TARGET_FOLDER="build_${BUILD_MODE}"
fi

echo "⚠️  This will remove: $TARGET_FOLDER"

if [[ -d "$TARGET_FOLDER" ]]; then
  rm -rf "$TARGET_FOLDER"
  echo "✅ Removed folder: $TARGET_FOLDER"
else
  echo "⚠️  Folder not found: $TARGET_FOLDER"
fi
