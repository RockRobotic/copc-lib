# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- **\[Python/C++\]** Add spatial function `Crosses` to `VoxelKey`
- **\[Python/C++\]** Add resolution function `Resolution` and `GetResolutionAtDepth` to `VoxelKey`
- **\[Python/C++\]** Add `Vector3` operators to `Vector3`
- **\[Python/C++\]** Add resolution query functions `GetDepthAtResolution`, `GetNodesAtResolution` and `GetNodesWithinResolution` to `Reader`
- **\[Python/C++\]** Add `BoundsTrimFileExample` and `ResolutionTrimFileExample` to example-writer files
- **\[Python/C++\]** Add `Classification` getter and setter to `Points`
- **\[Python/C++\]** Add `GetAllChildrenPoints` to `Reader`
- **\[Python/C++\]** Add `GetChildren` to `VoxelKey`
- **\[Python\]** Make `Node` and `VoxelKey` picklable

### Changed

- **\[Python/C++\]** Change order of arguments in `VoxelKey` spatial functions `Intersects`, `Contains`, and `Within`
- **\[Python/C++\]** Add optional `resolution` argument to `Reader` spatial query functions `GetNodesWithinBox`, `GetNodesIntersectBox`, `GetPointsWithinBox`, and `GetAllPoints` . `resolution` can be used to limit the resolution during spatial queries
- **\[Python/C++\]** Update `span` of `autzen-classified.copc.laz` test file from 0 to 128

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
[1.3.0]: https://github.com/RockRobotic/copc-lib/compare/v1.2.0...v1.3.0
[Unreleased]: https://github.com/RockRobotic/copc-lib/compare/v1.3.0...HEAD
