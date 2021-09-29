#!/bin/bash

set -ex

mkdir build
cd build

cmake -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DWITH_TESTS_AND_EXAMPLES=OFF \
  -DWITH_PYTHON=OFF \
  ..
  
cmake --build .
cmake --install .

cd $SRC_DIR

$PYTHON setup.py install
