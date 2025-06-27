#!/bin/bash

# Build BULLET targeting system

echo "Building BULLET targeting system..."

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_TARGETING=ON \
      -DENABLE_TESTS=ON \
      ..

make -j$(nproc) targeting_core

echo "Targeting system build complete."
