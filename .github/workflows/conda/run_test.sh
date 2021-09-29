#!/bin/bash

# catch2 uses features only available in osx >=10.12
export MACOSX_DEPLOYMENT_TARGET="10.12"

# build test files
mkdir "test/build"
cd "test/build"

cmake \
  -DCMAKE_BUILD_TYPE=Release \
  ..

cmake --build .
ctest

# copy python files into test build folder
cp ../*.py .
# run python tests
pytest


cd $SRC_DIR


# build example files
mkdir "example/build"
cd "example/build"

cmake \
  -DCMAKE_BUILD_TYPE=Release \
  ..  

cmake --build .
ctest

# copy python files into example build folder
cp ../*.py .
# run python tests
python example-reader.py
python example-writer.py
