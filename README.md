# copc-lib

copc-lib is a library which provides an easy-to-use reader ~~and writer~~ interface for [COPC](https://copc.io/) point clouds. The end goal of this project is to provide a complete interface for handling COPC files, without having to use other laz or las libraries.

## Installation

copc-lib depends on laz-perf 2.1.0 or greater. See the [laz-perf repo](https://github.com/hobu/laz-perf) for installation instructions.

With the dependencies installed, we can now build copc-lib:

```bash
git clone https://github.com/RockRobotic/copc-lib.git
cd copc-lib
mkdir build && cd build
cmake ..
make
sudo make install
```

## Usage

For common use cases, see the `example` folder for reference.

Most users will find that the `Copc` object provides enough functionality for them. The goal of the `Copc` api is to provide an interface between the user and the laz/las file data, so that the user can avoid using another library to parse the laz file.

```cpp
#include <copc-lib/copc_lib.hpp>

// Create a new Copc library object
Copc copc("MyCopcFile.copc.laz");

// Get the LAS header from the file
auto header = copc.GetHeader();

// Get the WKT from the file
std::string wkt = copc.GetWktData();

// Decompress the point data of a node for a given VoxelKey
VoxelKey loadKey(1, 1, 1, 1);

// If the key isn't found, point_vec will be empty.
std::vector<char> point_vec = copc.ReadNodeData(loadEntry);
```

You can include copc-lib with CMake or by `make install`ing copc-lib then setting the appropriate g++ flags:

```bash
g++ example-lib.cpp -o example -lcopc-lib -llazperf
```

If other library functionality is needed, feel free to open an issue to see about getting them added.

## Coming Soon
- [x] Basic C++ Reader Interface
- [ ] Add writer for COPC data
- [ ] Spatial querying for nodes (given spatial coordinates, retrieve the appropriate Entry object)
- [ ] Python bindings
- [ ] Javascript (emscripten) bindings
- [ ] Return Point structures from the reader rather than raw char* arrays, to support a more familiar laspy-like interface.

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
Please see [LICENSE.md](LICENSE.md)

## Credits
copc-lib is created and maintained by [Chris Lee](https://github.com/CCInc), [Leo Stanislas](https://github.com/leo-stan) and other members of [RockRobotic](https://github.com/RockRobotic).

The [COPC file format](https://copc.io) is created and maintained by HOBU Inc.
Some code has been adopted from [PDAL](https://github.com/PDAL/PDAL) and [lazperf](https://github.com/hobu/laz-perf), both of which are maintained by HOBU Inc.
