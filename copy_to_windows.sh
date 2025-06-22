#!/bin/bash

# Destination directory
DEST="/mnt/d/ComputationalGeometry/data"

cp -r data/csv "$DEST"
cp -r data/mesh "$DEST"

echo "Copied data to windows drive."
