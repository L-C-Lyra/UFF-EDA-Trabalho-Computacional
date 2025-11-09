#!/bin/bash

cd "$(dirname "$0")"

if [ -d "build-test" ]; then
    rm -rf build-test
fi

mkdir build-test
cd build-test

cmake -DBUILD_TESTS=ON .. && make

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

./testBinaryTree
./testBinarySearchTree  
./testLinkedList
./testBPlusTree