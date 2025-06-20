#!/bin/bash

SOURCE_FOLDER=${1:-src}  # default to 'src' if not passed
OUTPUT_FOLDER="build"
cmake -DCGAL_DIR=$PWD/libs/CGAL-6.0.1 -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=$PWD/libs/boost_1_88_0 -DSOURCE_FOLDER="$SOURCE_FOLDER" -DOUTPUT_FOLDER="$OUTPUT_FOLDER" -B "$OUTPUT_FOLDER"