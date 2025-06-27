#!/bin/bash

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

FOLDER=${1:-}                        # e.g., DelaunayTriangulation
BUILD_MODE=${2:-training}           # training (default) or inference
BUILD_TYPE_INPUT=${3:-release}      # release (default) or debug

# Normalize inputs
BUILD_MODE=$(echo "$BUILD_MODE" | tr '[:upper:]' '[:lower:]')
BUILD_TYPE_INPUT=$(echo "$BUILD_TYPE_INPUT" | tr '[:upper:]' '[:lower:]')

# Determine build folder
if [[ "$BUILD_TYPE_INPUT" == "debug" ]]; then
  BUILD_FOLDER="buildDebug_${BUILD_MODE}/$FOLDER"
else
  BUILD_FOLDER="build_${BUILD_MODE}/$FOLDER"
fi

echo -e "${CYAN}🏗️  Running make in: $BUILD_FOLDER${NC}"

if [[ -d "$BUILD_FOLDER" ]]; then
  if make -C "$BUILD_FOLDER"; then
    echo -e "${GREEN}✅ Make completed successfully in: $BUILD_FOLDER${NC}"
  else
    echo -e "${RED}❌ Make failed in: $BUILD_FOLDER${NC}"
    exit 1
  fi
else
  echo -e "${RED}❌ Build folder does not exist: $BUILD_FOLDER${NC}"
  echo -e "${YELLOW}⚠️  Please run the generation script first!${NC}"
  exit 1
fi
