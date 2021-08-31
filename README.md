# copc-lib

copc-lib is a library which provides an easy-to-use reader ~~and writer~~ interface for [COPC](https://copc.io/) point clouds. The end goal of this project is to provide a complete interface for handling COPC files, without having to use other laz or las libraries.

*Note: This library is in beta and API may change*

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

The `copc::Copc` object provides a complete interface for reading COPC files. For common use cases, use the `example` and `test` folders as reference.

```cpp
#include <copc-lib/copc/copc.hpp>
using namespace copc;

// Create a new Copc library object
Copc copc("MyCopcFile.copc.laz");

// Get the LAS header from the file
// The `file` object contains all metadata for both COPC and LAS formats
auto lasHeader = copc.file->GetLasHeader();

// Get the WKT from the file
std::string wkt = copc.file->GetWkt();

// Use VoxelKeys to query the hierarchy
hierarchy::VoxelKey loadKey(1, 1, 1, 1);

// FindNode queries the hierarchy to get the requested Entry
auto node = copc.hierarchy->FindNode(loadKey);
// Once we have the node, we can load it using GetPoints
std::vector<las::Point> nodeData = node->GetPoints();

// The las::Point interface gives us access to all LAS attributes
std::cout << nodeData[0].X() << ", " << nodeData[0].Y() << nodeData[0].Z();
```

You can see an example of g++ and cmake build processes [here](https://github.com/RockRobotic/copc-lib-examples).

If other library functionality is needed, feel free to open an issue to see about getting it added.

## Coming Soon
- [x] Basic C++ Reader Interface
- [x] Return Point structures from the reader rather than raw char* arrays, to support a more familiar laspy-like interface.
- [ ] Add writer for COPC data
- [ ] Spatial querying for nodes (given spatial coordinates, retrieve the appropriate Entry object)
- [ ] Python bindings
- [ ] Javascript (emscripten) bindings

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
Please see [LICENSE.md](LICENSE.md)

## Credits
copc-lib is created and maintained by [Chris Lee](https://github.com/CCInc), [Leo Stanislas](https://github.com/leo-stan) and other members of [RockRobotic](https://github.com/RockRobotic).

The [COPC file format](https://copc.io) is created and maintained by HOBU Inc.
Some code has been adopted from [PDAL](https://github.com/PDAL/PDAL) and [lazperf](https://github.com/hobu/laz-perf), both of which are maintained by HOBU Inc.
