# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [2.2.2] - 2022-01-31

### Fixed

- **\[Python/C++\]**  Fix bug with handling of non-existant path in FileWriter.
- **\[Python/C++\]**  Fix test causing i686 tests to fail.
- **\[Python\]**  Fix python ctest.

## [2.2.1] - 2022-01-24

### Added
- **\[Python\]** Pip release workflow

### Changed
- **\[Python\]** Python build uses scikit-build instead of custom solution

## [2.2.0] - 2022-01-10

### Added
- **\[Python/C++\]** Add `UnscaledX`, `UnscaledY`, `UnscaledZ` properties to `las::Points`

### Changed
- **\[Python/C++\]** Add `point_format_id` argument to `Writer` constructor to allow update of the point format between reader and writer.
- **\[Python/C++\]** Implement `CopcExtentVlr` for compatibility with lazperf 3.0.0
- **\[Python/C++\]** Update lazperf dependency version to 3.0.0

## [2.1.3] - 2021-11-17

### Changed

- **\[Python/C++\]** Update `CopcFileWriter` constructor to provide optional arguments to update information when using a `CopcConfig` object from a `Reader`. Optional arguments are `scale`, `offset`, `wkt`, `extra_bytes_vlr`, and `has_extended_stats`.
- **\[Python/C++\]** Make `LasHeader`'s `scale` and `offset` read-only.

## [2.1.2] - 2021-11-12

### Fixed

- **\[General\]**  Fix unintentional CMakeList version bump

## [2.1.1] - 2021-11-12

### Fixed

- **\[Python\]** Add pickling of `Node::page_key`

## [2.1.0] - 2021-11-11

### Added

- **\[Python/C++\]** Add `PageExists(VoxelKey key)` to `Writer`
- **\[Python/C++\]** Add `GetParentAtDepth(int32_t depth)` to `VoxelKey`
- **\[Python/C++\]** Add `GetPageList()` to `Reader`
- **\[Python/C++\]** Add `page` member to `Node`
- **\[Python/C++\]** Add `ChangeNodePage` to `Writer`

### Changed

- **\[Python/C++\]** Write WKT as EVLR instead of VLR to align with laspy
- **\[Python/C++\]** Remove the error when scan angle is not within LAS specs [-30000,30000]
- **\[Python/C++\]** !!!Potential Breakage!!! Replaced `Page page` argument with optional `VoxelKey page_key` in `Writer`'s `AddNode` and `AddNodeCompressed`. If no `page_key` is provided the node is added to the root page, if a `page_key` is provided then the node is added to that page. If the requested page doesn't exist, it is created using the nearest existing hierarchical parent as page parent.

## [2.0.0] - 2021-10-28

### Added

- **\[Python/C++\]** Add `CopcExtent` and `CopcExtents` classes
- **\[Python/C++\]** Add extended stats (mean/var) to `CopcExtent` using an additional Lazperf extents VLR (see README.md for more info).
- **\[Python/C++\]** Add `GetCopcExtents` function to `CopcConfig`
- **\[Python/C++\]** Add `GetCopcExtents` and `SetCopcExtents` functions to `Writer`
- **\[Python\]** Add `reader.extents` property
- **\[Python/C++\]** Add `CopcInfo` class
- **\[Python/C++\]** Add vector(c++)/list(python) constructor to `VoxelKey`
- **\[Python/C++\]** Add `CopcConfig` class
- **\[Python/C++\]** Add `GetCopcConfig` function to `BaseIO`
- **\[Python/C++\]** Support COPC Extents, WKT, and Extra Bytes as VLR or EVLR
- **\[Python/C++\]** Add `SetCopcExtents` and `SetCopcInfo` to `Writer`
- **\[Python/C++\]** Add `LasHeaderBase` class
- **\[Python/C++\]** Add `ToString` to `LasHeader` class
- **\[Python/C++\]** Add `VlrHeader` class
- **\[Python/C++\]** Add a check for `WKT` byte in `LasHeader::FromLazPerf`

### Changed

- **\[Python/C++\]** Supported LAS point formats are now strictly 6 to 8 throughout the library
- **\[Python/C++\]** Remove `point_count_14`, `points_by_return_14`, `header_size_`, `wave_offset`, and `version` from `LasHeader`
- **\[Python/C++\]** Change lazperf version requirement to > 2.1.0
- **\[Python/C++\]** Update autzen-classified.copc.laz test file to be latest official
- **\[Python/C++\]** Change use of `las::CopcVlr::span` to `CopcInfo::spacing`
- **\[Python/C++\]** Change `CopcConfig::GetWkt` return type from `las::WktVlr` to `std::string`
- **\[Python/C++\]** Rename `CopcConfig::GetCopc` to `CopcConfig::GetCopcInfo` and now returns a `CopcInfo` class
- **\[Python/C++\]** Rename `Reader::GetCopcHeader` to `Reader::GetCopcInfo` and now returns a `CopcInfo` class
- **\[Python/C++\]** `VoxelKey::Resolution` and `VoxelKey::GetResolutionAtDepth`  now take `CopcInfo` argument instead of `las::CopcVlr`
- **\[Python/C++\]** Remove `Writer::LasConfig`, now using `CopcConfig` instead.
- **\[Python/C++\]** `Writer` and `FileWriter` constructors now takes a `CopcConfig` class as argument
- **\[Python/C++\]** Rename `NumExtraBytes` to `EbByteSize`
- **\[Python\]** Change `reader.GetCopcHeader()` function to `reader.copc_info` property
- **\[Python\]** Change `reader.CopcConfig().LasHeader()` function to `reader.las_header` property
- **\[Python\]** Change `reader.CopcConfig().Wkt()` function to `reader.wkt` property
- **\[Python\]** Change `reader.CopcConfig().ExtraBytesVlr()` function to `reader.extra_bytes_vlr` property
- **\[C++\]** Add `create_test_data.py` and `create_test_data.sh` to create data for `writer_node_test.cpp`
- **\[Python\]** Make `VoxelKey.BaseKey` and `VoxelKey.InvalidKey` static functions
- **\[Python\]** Update `Point` and `Points` property names from `CamelCase` to `under_score`
- **\[Python/C++\]** Rename `Reader::GetAllChildren(VoxelKey)` to `Reader::GetAllChildrenOfPage(VoxelKey)`
- **\[Python/C++\]** Rename `Reader::GetAllChildren()` to `Reader::GetAllNodes()`
- **\[C++\]** Rename `PointFormatID` to `PointFormatId`
- **\[C++\]** Rename `PointSourceID` to `PointSourceId`
- **\[C++\]** Rename `HasRGB` and `HasNIR` to `HasRgb` and `HasNir` respectively
- **\[C++\]** Rename `NIR` to `Nir`
- **\[Python/C++\]** Rename `ScanAngleFloat` to `ScanAngleDegrees`
- **\[Python\]** Rename `unscaled_x`, `unscaled_y`, `unscaled_z` to `x`, `y`, `z`
- **\[General\]** Rename CMake flag `WITH_TESTS_AND_EXAMPLES` to `WITH_TESTS`
- **\[Python/C++\]** Rename `Box::ZeroBox` to `Box::EmptyBox`
- **\[Python/C++\]** Rename `VoxelKey::BaseKey` to `VoxelKey::RootKey`

## [1.3.1] - 2021-10-19

### Added

- **\[Python/C++\]** Add spatial function `Crosses` to `VoxelKey`
- **\[Python/C++\]** Add resolution function `Resolution` and `GetResolutionAtDepth` to `VoxelKey`
- **\[Python/C++\]** Add `Vector3` operators to `Vector3`
- **\[Python/C++\]** Add resolution query functions `GetDepthAtResolution`, `GetNodesAtResolution` and `GetNodesWithinResolution` to `Reader`
- **\[Python/C++\]** Add `BoundsTrimFileExample` and `ResolutionTrimFileExample` to example-writer files
- **\[Python/C++\]** Add `Classification` and `PointSourceID` getter and setter to `Points`
- **\[Python/C++\]** Add `GetChildren` to `VoxelKey`
- **\[Python\]** Make `Node` and `VoxelKey` picklable
- **\[Python/C++\]** Add a new constructor to `Box` using `Vector3`
- **\[Python/C++\]** Add scaling functions to `LasHeader`
- **\[Python/C++\]** Add `ValidateSpatialBounds` function to `Reader`

### Changed

- **\[Python/C++\]** Change order of arguments in `VoxelKey` spatial functions `Intersects`, `Contains`, and `Within`
- **\[Python/C++\]** Add optional `resolution` argument to `Reader` spatial query functions `GetNodesWithinBox`, `GetNodesIntersectBox`, `GetPointsWithinBox`, and `GetAllPoints` . `resolution` can be used to limit the resolution during spatial queries
- **\[Python/C++\]** Update `span` of `autzen-classified.copc.laz` test file from 0 to 128
- **\[Python/C++\]** Rename `ExtendedReturnsBitFields` to `ReturnsBitField` and `ExtendedFlagsBitFields` to `FlagsBitField` in `Point` class
- **\[Python/C++\]** Make WKT VLR optional

### Fixed

- **\[Python\]** Fix typo in pickling of `Node`
- **\[Python\]** Fix scale error and reformat `NewFileExample()` in example_writer.py and .cpp

## [1.3.0] - 2021-10-04

### Added

- **\[Python/C++\]** Indexing directly on Points object in C++/python, including slicing support in python
- **\[Python\]** Pickle support for `LasHeader`, `Vector3`
- **\[Python/C++\]** Update header min, max, and points by return after the writer has been created
- **\[Python/C++\]** Add spatial functions `Intersect`, `Contains`, `Within` to `Box`, `VoxelKey`
- **\[Python/C++\]** Add spatial function `Within` to `Point`, `Points`
- **\[Python/C++\]** Add query functions `GetNodesWithinBox` and `GetPointsWithinBox` to Reader
- **\[C++\]** Support for Windows build
- **\[General\]** Add CHANGELOG.md to track changes

### Changed

- **\[Python\]** `VoxelKey` is implicitly convertible to a python tuple
- **\[Python/C++\]** `las::Points` can be indexed directly with `[]`
- **\[Python/C++\]** `las::Points` holds shared pointers of point objects, so changes to the `Point` object within the `Points` container are properly reflected
- **\[Python\]** `points.Size()` has been removed in python, replaced by `len(points)`
- **\[Python\]** `points.Get()` and `points.Get(i)` have been replaced by `__getitem__` and `__iter__`
- **\[C++\]** `las::Point` objects are now solely referenced by shared pointers, so pointer accessors must be used
- **\[C++\]** CMakeLists have been restructured to allow for building python/test/examples independently of the main library
- **\[C++\]** The copc-lib library now must be referenced with the `COPCLIB::` namespace in CMake

## [1.2.0] - 2021-09-17

### Added

- Release-it CI for releases
- Python bindings
- Points class for holding multiple "Point"s
- File path reader/writers
- Point scaling based on scale/offset

### Changed

- `Point::X` was changed to refer to `Point::UnscaledX` (same for Y and Z). `Point::X` now refers to the scaled and offsetted point.

## [1.1.0] - 2021-09-08

### Added

- COPC file writer to create new files
- Point class to parse LAS points

### Removed

- `Copc` class

[1.1.0]: https://github.com/RockRobotic/copc-lib/compare/v1.0...v1.1.0
[1.2.0]: https://github.com/RockRobotic/copc-lib/compare/v1.1.0...v1.2.0
[2.2.2]: https://github.com/RockRobotic/copc-lib/compare/v2.2.1...v2.2.2
[2.2.1]: https://github.com/RockRobotic/copc-lib/compare/v2.2.0...v2.2.1
[2.2.0]: https://github.com/RockRobotic/copc-lib/compare/v2.1.3...v2.2.0
[2.1.3]: https://github.com/RockRobotic/copc-lib/compare/v2.1.2...v2.1.3
[2.1.2]: https://github.com/RockRobotic/copc-lib/compare/v2.1.1...v2.1.2
[2.1.1]: https://github.com/RockRobotic/copc-lib/compare/v2.1.0...v2.1.1
[2.1.0]: https://github.com/RockRobotic/copc-lib/compare/v2.0.0...v2.1.0
[2.0.0]: https://github.com/RockRobotic/copc-lib/compare/v1.3.1...v2.0.0
[1.3.1]: https://github.com/RockRobotic/copc-lib/compare/v1.3.0...v1.3.1
[1.3.0]: https://github.com/RockRobotic/copc-lib/compare/v1.2.0...v1.3.0
[Unreleased]: https://github.com/RockRobotic/copc-lib/compare/v2.2.2...HEAD
