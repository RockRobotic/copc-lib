# copc-lib

copc-lib is a library which provides an easy-to-use reader and writer interface for [COPC](https://copc.io/) point clouds. This project provides a complete interface for handling COPC files, so that no additional LAZ or LAS libraries are required.

*Note: This library is in beta and API may change*

## Build from source

### Dependencies

copc-lib has the following dependencies:

- laz-perf >= [commit 4819611](https://github.com/hobu/laz-perf/commits/4819611b279cb791508a0ac0cedd913f8c1d2103)
- Catch2
- Pybind11

To build the latest version of laz-perf:

```bash
git clone https://github.com/hobu/laz-perf.git
cd laz-perf
git checkout 4819611b279cb791508a0ac0cedd913f8c1d2103
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
sudo cmake --install .
```

### C++

```bash
git clone https://github.com/RockRobotic/copc-lib.git
cd copc-lib
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```

### Python

```bash
git clone https://github.com/RockRobotic/copc-lib.git
pip install ./copc-lib
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

#### Example Files & Unit Tests

To build the copc-lib examples and unit tests along with the main library, you must enable them:

```bash
mkdir build && cd build
cmake .. -DWITH_TESTS_AND_EXAMPLES=ON
cmake --build .
```

Then, you can run the unit tests and the examples:

```bash
ctest # All tests should pass

cd bin
./example_reader
./example_writer
```

Alternatively, you can build the test and example files from their respective CMakeLists, assuming copc-lib is already installed.

### Python

```python
import copclib as copc

# Create a reader object
reader = copc.FileReader("autzen-classified.copc.laz")

# Get the node metadata from the hierarchy
node = reader.FindNode(copc.VoxelKey(0, 0, 0, 0))
# Fetch the points of a node
points = reader.GetPoints(node)

# Iterate through each point
for point in points.Get():
    print(point)
```

## Coming Soon

- \[x\] Basic C++ Reader Interface
- \[x\] Return Point structures from the reader rather than raw char\* arrays, to support a more familiar laspy-like interface.
- \[x\] Add writer for COPC data
- \[x\] Python bindings
- \[x\] JavaScript bindings (not planned, see below)
- \[ \] Conda and pip packages
- \[ \] Spatial querying for nodes (given spatial coordinates, retrieve the appropriate Entry object)

## Note

This version of copc-lib is pinned to a draft version of COPC respective of the state at [COPC.io](https://github.com/copcio/copcio.github.io/tree/a6e8654f65db7c7d438ebea90993bd7a8d59091a).

## Helpful Links

- [COPC Spec](https://copc.io/)
- [copc.js](https://github.com/connormanning/copc.js) - TypeScript library for reading COPC files
- [copc.js for browser](https://github.com/connormanning/copc.js/pull/1) - Webpacked version of copc.js for the browser

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

### Naming Convention

#### C++

We mostly use Google's [Style Guide](https://google.github.io/styleguide/cppguide.html).
```c++

namespace name
{
class ClassName
{
public:
// Default constructor
ClassName() = default;
ClassName(int public_variable, int private_variable, bool private_read_only)
: public_variable(public_variable), private_variable_(private_variable),
private_read_only_(private_read_only){};

int public_variable{};

// Getters and Setters
void PrivateVariable(int private_variable) { private_variable_ = private_variable; }
int PrivateVariable() const { return private_variable_; }
bool PrivateReadOnly() const { return private_read_only_; }

// Any other function
int PrivateVariablePlusOne() { return private_variable_ + 1; }
int SumPublicAndPrivate() const { return public_variable + private_variable_; }
static std::string ReturnEmptyString() { return {}; }

private:
int private_variable_{};
bool private_read_only_{false};
};
} // namespace name
```

#### Python
```python
test_class = ClassName(public_variable=1,private_variable=2,private_read_only=True)

# using pybind .def_readwrite
test_class.public_variable = 4
assert test_class.public_variable == 4

# using pybind .def_property
test_class.private_variable = 5
assert test_class.private_variable == 5

# using pybind .def_property_readonly
assert test_class.private_read_only == True

# using pybind .def
assert test_class.PrivateVariablePlusOne() == 6
assert test_class.SumPublicAndPrivate() == 9

# using pybind .def_static
assert test_class.ReturnEmptyString == ""
```

## License

Please see [LICENSE.md](LICENSE.md)

## Credits

copc-lib is created and maintained by [Chris Lee](https://github.com/CCInc), [Leo Stanislas](https://github.com/leo-stan) and other members of [RockRobotic](https://github.com/RockRobotic).

The [COPC file format](https://copc.io) is created and maintained by HOBU Inc.
Some code has been adopted from [PDAL](https://github.com/PDAL/PDAL) and [lazperf](https://github.com/hobu/laz-perf), both of which are maintained by HOBU Inc.
