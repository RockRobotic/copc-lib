# copc-lib

copc-lib provides an easy-to-use reader and writer interface for [COPC](https://copc.io/) point clouds, with bindings for python and C++.

## Installation

The quickest way to get started with copc-lib is with our conda and pip packages.

### Conda
Conda includes both the C++ and python bindings:

```bash
conda install -c conda-forge copc-lib
```

### Pip
Pip provide only python bindings:

```bash
pip install copclib
```

### Building from source

#### Dependencies

copc-lib has the following dependencies:

- laz-perf>=[3.0.0](https://github.com/hobu/laz-perf/releases/tag/3.0.0)
- Catch2 v2.x (test suite only)
- Pybind11 (python bindings only)

To install all dependencies:
```bash
conda install -c conda-forge "laz-perf>=3.0" Catch2=2.13 pybind11
```

#### C++

```bash
git clone https://github.com/RockRobotic/copc-lib.git
cd copc-lib
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

#### Python

```bash
git clone https://github.com/RockRobotic/copc-lib.git
pip install ./copc-lib
```

#### Example Files & Unit Tests

To build the copc-lib examples and unit tests along with the main library, you must enable them:

```bash
mkdir build && cd build
cmake .. -DWITH_TESTS=ON -DWITH_PYTHON=ON
cmake --build .
ctest # All tests should pass
```

## Usage

The `Reader` and `Writer` objects provide the primary means of interfacing with your COPC files. For more complex use cases, we also provide additional objects such as LAZ Compressors and Decompressors (see [example/example-writer.cpp](example/example-writer.cpp)).

For common use cases, see the `example` and `test` folders for full examples.

### C++

copc-lib is compatible with CMake. Assuming copc-lib and lazperf are installed on the system, you can link with them in your `CMakeLists.txt`:

```CMake
find_package(COPCLIB REQUIRED)
find_package(LAZPERF REQUIRED)

add_executable(funlib fun-main.cpp)
target_link_libraries(funlib COPCLIB::copc-lib LAZPERF::lazperf)
```

The primary public interface will be your [FileReader](./cpp/include/copc-lib/io/copc_reader.hpp) and [FileWriter](./cpp/include/copc-lib/io/copc_writer.hpp) objects. Check the headers and [example files](./example) for documentation.

```cpp
#include <iostream>
#include <copc-lib/io/reader.hpp>

void main()
{
    // Create a reader object
    FileReader reader("autzen-classified.copc.laz");

    // Get the node metadata from the hierarchy
    auto node = reader.FindNode(copc.VoxelKey(0, 0, 0, 0));
    // Fetch the points of a node
    auto points = reader.GetPoints(node);

    // Iterate through each point
    for (const auto &point : points)
        std::cout << "X: " << point.X ", Y: " << point.Y << ", Z: " << point.Z  << std::endl;
}
```

### Python

[Example files](./example) are also provided for python.

```python
import copclib as copc

# Create a reader object
reader = copc.FileReader("autzen-classified.copc.laz")

# Get the node metadata from the hierarchy
node = reader.FindNode(copc.VoxelKey(0, 0, 0, 0))
# Fetch the points of a node
points = reader.GetPoints(node)

# Iterate through each point
for point in points:
    print(point.x, point.y, point.z)
```

Note that, in python, dimension names for points follow the [laspy naming scheme](https://laspy.readthedocs.io/en/latest/intro.html#point-format-6), with the exception of `scan_angle`.

## Helpful Links

- [COPC Spec](https://copc.io/)
- [copc.js](https://github.com/connormanning/copc.js) - TypeScript library for reading COPC files
- [copc.js for browser](https://github.com/connormanning/copc.js/pull/1) - Webpacked version of copc.js for the browser

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

Please see [LICENSE.md](LICENSE.md)

## Credits

copc-lib is created and maintained by [Chris Lee](https://github.com/CCInc), [Leo Stanislas](https://github.com/leo-stan) and other members of [RockRobotic](https://github.com/RockRobotic).

The [COPC file format](https://copc.io) is created and maintained by HOBU Inc.
Some code has been adopted from [PDAL](https://github.com/PDAL/PDAL) and [lazperf](https://github.com/hobu/laz-perf), both of which are maintained by HOBU Inc.
