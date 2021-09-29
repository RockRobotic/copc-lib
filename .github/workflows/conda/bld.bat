@ECHO ON

mkdir build
cd build

echo %SRC_DIR%
echo %LIBRARY_PREFIX%

set CMAKE_GENERATOR=NMake Makefiles

cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% ^
  -DBUILD_SHARED_LIBS=ON ^
  -DWITH_TESTS_AND_EXAMPLES=OFF ^
  -DWITH_PYTHON=OFF ^
  %SRC_DIR%
type CMakeFiles\CMakeOutput.log
if errorlevel 1 exit 1

cmake --build . --config Release --target INSTALL -- VERBOSE=1
if errorlevel 1 exit 1

cd %SRC_DIR%
%PYTHON% setup.py install
if errorlevel 1 exit 1