@echo on

set CMAKE_GENERATOR=NMake Makefiles

REM build test files
mkdir "test/build"
cd "test/build"

cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  ..
if errorlevel 1 exit 1

cmake --build . --config Release
if errorlevel 1 exit 1

ctest -C Release
if errorlevel 1 exit 1

REM copy python files into test build folder
copy /y ..\*.py .
if errorlevel 1 exit 1
REM run python tests
pytest
if errorlevel 1 exit 1


cd %SRC_DIR%
if errorlevel 1 exit 1


REM build example files
mkdir "example/build"
cd "example/build"
if errorlevel 1 exit 1

cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  ..
if errorlevel 1 exit 1

cmake --build . --config Release
if errorlevel 1 exit 1

ctest -C Release
if errorlevel 1 exit 1

REM copy python files into example build folder
copy /y ..\*.py .
if errorlevel 1 exit 1
REM run python tests
python example-reader.py
if errorlevel 1 exit 1
python example-writer.py
if errorlevel 1 exit 1
