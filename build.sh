#!/bin/bash

cd "$(dirname "$0")"

if [ -d "build" ]; then
    rm -rf build
fi

mkdir build
cd build

cmake .. && make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run ./build/UFF_EDA_Trabalho_Computacional to execute"
else
    echo "Build failed!"
    exit 1
fi