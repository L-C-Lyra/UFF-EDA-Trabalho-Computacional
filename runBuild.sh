#!/bin/bash


cd "$(dirname "$0")"
if [ -d "build" ]; then
    rm -rf build
fi
mkdir build
cd build
cmake .. && make

if [ $? -eq 0 ]; then
    ./UFF_EDA_Trabalho_Computacional
else
    echo "Build failed!"
    exit 1
fi