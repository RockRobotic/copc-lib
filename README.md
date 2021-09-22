# copc-lib

copc-lib is a library which provides an easy-to-use reader and writer interface for [COPC](https://copc.io/) point clouds. This project provides a complete interface for handling COPC files, so that no additional LAZ or LAS libraries are required.

*Note: This library is in beta and API may change*

## Installation

### Dependencies
copc-lib depends on laz-perf 2.1.0 or greater. See the [laz-perf repo](https://github.com/hobu/laz-perf) for installation instructions.

### Build & Install
With the dependencies installed, we can now build copc-lib. Note that you need to clone this repo with the `--recurse-submodules` flag, so that all dependencies get included.

### C++

```bash
git clone --recurse-submodules https://github.com/RockRobotic/copc-lib.git
cd copc-lib
mkdir build && cd build
cmake ..
make
sudo make install
```

### Python

```bash
git clone --recurse-submodules https://github.com/RockRobotic/copc-lib.git
pip install ./copc-lib
```

## Usage

The `Reader` and `Writer` objects provide the primary means of interfacing with your COPC files. For more complex use cases, we also provide additional objects such as LAZ Compressors and Decompressors (see [example/example-writer.cpp](example/example-writer.cpp)).

For common use cases, see the `example` and `test` folders for full examples.

### C++
copc-lib is compatible with CMake. Assuming copc-lib and lazperf are installed on the system, you can link with them in your `CMakeLists.txt`:

```CMake
find_package(copc-lib REQUIRED)
find_package(lazperf REQUIRED)

add_executable(funlib fun-main.cpp)
target_link_libraries(funlib copc-lib LAZPERF::lazperf)
```

### Python
```python
import copclib as copc

# Create a reader object
reader = copc.FileReader("../test/data/autzen-classified.copc.laz")

# Get the node metadata from the hierarchy
node = reader.FindNode(copc.VoxelKey(0, 0, 0, 0))
# Fetch the points of a node
points = reader.GetPoints(node)

# Iterate through each point
for point in points.Get():
    print(point)
```

## Coming Soon
- [x] Basic C++ Reader Interface
- [x] Return Point structures from the reader rather than raw char* arrays, to support a more familiar laspy-like interface.
- [x] Add writer for COPC data
- [x] Python bindings
- [x] JavaScript bindings (not planned, see below)
- [ ] Conda and pip packages
- [ ] Spatial querying for nodes (given spatial coordinates, retrieve the appropriate Entry object)

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
