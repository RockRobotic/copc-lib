#include <cmath>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <copc-lib/copc/extents.hpp>
#include <copc-lib/copc/info.hpp>
#include <copc-lib/geometry/box.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/io/copc_reader.hpp>
#include <copc-lib/io/copc_writer.hpp>
#include <copc-lib/io/laz_reader.hpp>
#include <copc-lib/io/laz_writer.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>
#include <copc-lib/las/vlr.hpp>
#include <copc-lib/laz/compressor.hpp>
#include <copc-lib/laz/decompressor.hpp>

namespace py = pybind11;
using namespace copc;

PYBIND11_MAKE_OPAQUE(std::vector<char>)

PYBIND11_MODULE(_core, m)
{
    py::bind_vector<std::vector<char>>(m, "VectorChar", py::buffer_protocol())
        .def(py::pickle(
            [](const std::vector<char> &vec) { // __getstate__
                // Convert vector<char> to string for pickling
                return py::make_tuple(py::bytes(std::string(vec.begin(), vec.end())));
            },
            [](const py::tuple &t) { // __setstate__
                auto s = t[0].cast<std::string>();
                // Convert string back to vector<char> for unpickling
                return std::vector<char>(s.begin(), s.end());
            }));

    py::class_<VoxelKey>(m, "VoxelKey")
        .def(py::init<>())
        .def(py::init<const int32_t &, const int32_t &, const int32_t &, const int32_t &>(), py::arg("d"), py::arg("x"),
             py::arg("y"), py::arg("z"))
        .def(py::init<const std::vector<int32_t> &>(), py::arg("list"))
        .def(py::self == py::self)
        .def_readwrite("d", &VoxelKey::d)
        .def_readwrite("x", &VoxelKey::x)
        .def_readwrite("y", &VoxelKey::y)
        .def_readwrite("z", &VoxelKey::z)
        .def("IsValid", &VoxelKey::IsValid)
        .def_static("RootKey", &VoxelKey::RootKey)
        .def_static("InvalidKey", &VoxelKey::InvalidKey)
        .def("Bisect", &VoxelKey::Bisect)
        .def("GetChildren", &VoxelKey::GetChildren)
        .def("GetParent", &VoxelKey::GetParent)
        .def("GetParentAtDepth", &VoxelKey::GetParentAtDepth, py::arg("depth"))
        .def("GetParents", &VoxelKey::GetParents, py::arg("include_current"))
        .def("ChildOf", &VoxelKey::ChildOf, py::arg("parent_key"))
        .def("Resolution", &VoxelKey::Resolution, py::arg("las_header"), py::arg("copc_header"))
        .def_static("GetResolutionAtDepth", &VoxelKey::GetResolutionAtDepth, py::arg("depth"), py::arg("las_header"),
                    py::arg("copc_header"))
        .def("Intersects", &VoxelKey::Intersects)
        .def("Contains", py::overload_cast<const las::LasHeader &, const Box &>(&VoxelKey::Contains, py::const_))
        .def("Contains", py::overload_cast<const las::LasHeader &, const Vector3 &>(&VoxelKey::Contains, py::const_))
        .def("Within", &VoxelKey::Within)
        .def("Crosses", &VoxelKey::Crosses)
        .def(py::hash(py::self))
        .def("__str__", &VoxelKey::ToString)
        .def("__repr__", &VoxelKey::ToString)
        .def(py::pickle(
            [](const VoxelKey &key) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(key.d, key.x, key.y, key.z);
            },
            [](const py::tuple &t) { // __setstate__
                if (t.size() != 4)
                    throw std::runtime_error("Invalid state!");

                /* Create a new C++ instance */
                VoxelKey key;
                key.d = t[0].cast<int32_t>();
                key.x = t[1].cast<int32_t>();
                key.y = t[2].cast<int32_t>();
                key.z = t[3].cast<int32_t>();
                return key;
            }));
    py::implicitly_convertible<py::tuple, VoxelKey>();

    py::class_<Box>(m, "Box")
        .def(py::init<>())
        .def(py::init<const double &, const double &, const double &, const double &, const double &, const double &>(),
             py::arg("x_min"), py::arg("y_min"), py::arg("z_min"), py::arg("x_max"), py::arg("y_max"), py::arg("z_max"))
        .def(py::init<const double &, const double &, const double &, const double &>(), py::arg("x_min"),
             py::arg("y_min"), py::arg("x_max"), py::arg("y_max"))
        .def(py::init<const Vector3 &, const Vector3 &>(), py::arg("min"), py::arg("max"))
        .def(py::init<const std::vector<double> &>(), py::arg("list"))
        .def(py::init<const VoxelKey &, const las::LasHeader &>(), py::arg("key"), py::arg("las_header"))
        .def_readwrite("x_min", &Box::x_min)
        .def_readwrite("y_min", &Box::y_min)
        .def_readwrite("z_min", &Box::z_min)
        .def_readwrite("x_max", &Box::x_max)
        .def_readwrite("y_max", &Box::y_max)
        .def_readwrite("z_max", &Box::z_max)
        .def_static("MaxBox", &Box::MaxBox)
        .def_static("EmptyBox", &Box::EmptyBox)
        .def("Intersects", &Box::Intersects)
        .def("Contains", py::overload_cast<const Box &>(&Box::Contains, py::const_))
        .def("Contains", py::overload_cast<const Vector3 &>(&Box::Contains, py::const_))
        .def("Within", &Box::Within)
        .def("__str__", &Box::ToString)
        .def("__repr__", &Box::ToString);

    py::implicitly_convertible<py::tuple, Box>();

    py::class_<Node>(m, "Node")
        .def(py::init<>())
        .def_readwrite("point_count", &Node::point_count)
        .def_readwrite("key", &Node::key)
        .def_readwrite("offset", &Node::offset)
        .def_readwrite("byte_size", &Node::byte_size)
        .def_readwrite("page_key", &Node::page_key)
        .def("IsValid", &Node::IsValid)
        .def("IsPage", &Node::IsPage)
        .def("__str__", &Node::ToString)
        .def("__repr__", &Node::ToString)
        .def(py::pickle(
            [](const Node &n) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(n.key, n.offset, n.byte_size, n.point_count, n.page_key);
            },
            [](const py::tuple &t) { // __setstate__
                if (t.size() != 5)
                    throw std::runtime_error("Invalid state!");

                /* Create a new C++ instance */
                Node node;
                node.key = t[0].cast<VoxelKey>();
                node.offset = t[1].cast<uint64_t>();
                node.byte_size = t[2].cast<int32_t>();
                node.point_count = t[3].cast<int32_t>();
                node.page_key = t[4].cast<VoxelKey>();
                return node;
            }));

    py::class_<Page>(m, "Page")
        .def_readwrite("point_count", &Page::point_count)
        .def_readwrite("key", &Page::key)
        .def_readwrite("offset", &Page::offset)
        .def_readwrite("byte_size", &Page::byte_size)
        .def_readwrite("loaded", &Page::loaded)
        .def("IsValid", &Page::IsValid)
        .def("IsPage", &Page::IsPage)
        .def("__str__", &Page::ToString)
        .def("__repr__", &Page::ToString);

    py::class_<Vector3>(m, "Vector3")
        .def(py::init<>())
        .def(py::init<const double &, const double &, const double &>(), py::arg("x"), py::arg("y"), py::arg("z"))
        .def(py::init<const std::vector<double> &>(), py::arg("list"))
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        .def_static("DefaultScale", &Vector3::DefaultScale)
        .def_static("DefaultOffset", &Vector3::DefaultOffset)
        .def(py::self == py::self)
        // Vector3 operations
        .def(
            "__mul__", [](const Vector3 &vec, const Vector3 &other) { return vec * other; }, py::is_operator())
        .def(
            "__truediv__", [](const Vector3 &vec, const Vector3 &other) { return vec / other; }, py::is_operator())
        .def(
            "__floordiv__",
            [](const Vector3 &vec, const Vector3 &other)
            { return Vector3(std::floor(vec.x / other.x), std::floor(vec.y / other.y), std::floor(vec.z / other.z)); },
            py::is_operator())
        .def(
            "__add__", [](const Vector3 &vec, const Vector3 &other) { return vec + other; }, py::is_operator())
        .def(
            "__sub__", [](const Vector3 &vec, const Vector3 &other) { return vec - other; }, py::is_operator())
        // Double operations
        .def(
            "__mul__", [](const Vector3 &vec, const double &d) { return vec * d; }, py::is_operator())
        .def(
            "__truediv__", [](const Vector3 &vec, const double &d) { return vec / d; }, py::is_operator())
        .def(
            "__floordiv__",
            [](const Vector3 &vec, const double &d)
            { return Vector3(std::floor(vec.x / d), std::floor(vec.y / d), std::floor(vec.z / d)); },
            py::is_operator())
        .def(
            "__add__", [](const Vector3 &vec, const double &d) { return vec + d; }, py::is_operator())
        .def(
            "__sub__", [](const Vector3 &vec, const double &d) { return vec - d; }, py::is_operator())
        .def("__str__", &Vector3::ToString)
        .def("__repr__", &Vector3::ToString)
        .def(py::pickle(
            [](const Vector3 &vec) { // __getstate__
                return py::make_tuple(vec.x, vec.y, vec.z);
            },
            [](const py::tuple &t) { // __setstate__
                return Vector3(t[0].cast<double>(), t[1].cast<double>(), t[2].cast<double>());
            }));

    py::implicitly_convertible<py::list, Vector3>();
    py::implicitly_convertible<py::tuple, Vector3>();

    py::class_<las::Point, std::shared_ptr<las::Point>>(m, "Point")
        .def_property("x", py::overload_cast<>(&las::Point::X, py::const_),
                      py::overload_cast<const double &>(&las::Point::X))
        .def_property("y", py::overload_cast<>(&las::Point::Y, py::const_),
                      py::overload_cast<const double &>(&las::Point::Y))
        .def_property("z", py::overload_cast<>(&las::Point::Z, py::const_),
                      py::overload_cast<const double &>(&las::Point::Z))
        .def_property("intensity", py::overload_cast<>(&las::Point::Intensity, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Intensity))
        .def_property("returns_bit_field", py::overload_cast<>(&las::Point::ReturnsBitField, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ReturnsBitField))
        .def_property("return_number", py::overload_cast<>(&las::Point::ReturnNumber, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ReturnNumber))
        .def_property("number_of_returns", py::overload_cast<>(&las::Point::NumberOfReturns, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::NumberOfReturns))
        .def_property("flags_bit_field", py::overload_cast<>(&las::Point::FlagsBitField, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::FlagsBitField))
        .def_property("synthetic", py::overload_cast<>(&las::Point::Synthetic, py::const_),
                      py::overload_cast<const bool &>(&las::Point::Synthetic))
        .def_property("key_point", py::overload_cast<>(&las::Point::KeyPoint, py::const_),
                      py::overload_cast<const bool &>(&las::Point::KeyPoint))
        .def_property("withheld", py::overload_cast<>(&las::Point::Withheld, py::const_),
                      py::overload_cast<const bool &>(&las::Point::Withheld))
        .def_property("overlap", py::overload_cast<>(&las::Point::Overlap, py::const_),
                      py::overload_cast<const bool &>(&las::Point::Overlap))
        .def_property("scanner_channel", py::overload_cast<>(&las::Point::ScannerChannel, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ScannerChannel))
        .def_property("scan_direction_flag", py::overload_cast<>(&las::Point::ScanDirectionFlag, py::const_),
                      py::overload_cast<const bool &>(&las::Point::ScanDirectionFlag))
        .def_property("edge_of_flight_line", py::overload_cast<>(&las::Point::EdgeOfFlightLineFlag, py::const_),
                      py::overload_cast<const bool &>(&las::Point::EdgeOfFlightLineFlag))
        .def_property("classification", py::overload_cast<>(&las::Point::Classification, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::Classification))
        .def_property("scan_angle", py::overload_cast<>(&las::Point::ScanAngle, py::const_),
                      py::overload_cast<const int16_t &>(&las::Point::ScanAngle))
        .def_property("scan_angle_degrees", py::overload_cast<>(&las::Point::ScanAngleDegrees, py::const_),
                      py::overload_cast<const float &>(&las::Point::ScanAngleDegrees))
        .def_property("user_data", py::overload_cast<>(&las::Point::UserData, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::UserData))
        .def_property("point_source_id", py::overload_cast<>(&las::Point::PointSourceId, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::PointSourceId))
        .def_property("rgb", nullptr, py::overload_cast<const std::vector<uint16_t> &>(&las::Point::Rgb))
        .def_property("red", py::overload_cast<>(&las::Point::Red, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Red))
        .def_property("green", py::overload_cast<>(&las::Point::Green, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Green))
        .def_property("blue", py::overload_cast<>(&las::Point::Blue, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Blue))
        .def_property("gps_time", py::overload_cast<>(&las::Point::GPSTime, py::const_),
                      py::overload_cast<const double &>(&las::Point::GPSTime))
        .def_property("nir", py::overload_cast<>(&las::Point::Nir, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Nir))
        .def_property_readonly("point_format_id", &las::Point::PointFormatId)
        .def_property_readonly("point_record_length", &las::Point::PointRecordLength)
        .def_property("extra_bytes", py::overload_cast<>(&las::Point::ExtraBytes, py::const_),
                      py::overload_cast<const std::vector<uint8_t> &>(&las::Point::ExtraBytes))
        .def("HasRgb", &las::Point::HasRgb)
        .def("HasNir", &las::Point::HasNir)
        .def("ToPointFormat", &las::Point::ToPointFormat, py::arg("point_format_id"))
        .def("EbByteSize", &las::Point::EbByteSize)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("Within", &las::Point::Within, py::arg("box"))
        .def("__str__", &las::Point::ToString)
        .def("__repr__", &las::Point::ToString);

    using DiffType = ssize_t;
    using SizeType = size_t;

    auto wrap_i = [](DiffType i, SizeType n)
    {
        if (i < 0)
            i += n;
        if (i < 0 || (SizeType)i >= n)
            throw py::index_error();
        return i;
    };

    py::class_<las::Points>(m, "Points")
        .def(py::init<const uint8_t &, const uint16_t &>(),
             py::arg("point_format_id"), py::arg("eb_byte_size") = 0)
        .def(py::init<const std::vector<std::shared_ptr<las::Point>> &>(), py::arg("points"))
        .def(py::init<const las::LasHeader &>())
        .def_property("x", py::overload_cast<>(&las::Points::X, py::const_),
                      py::overload_cast<const std::vector<double> &>(&las::Points::X))
        .def_property("y", py::overload_cast<>(&las::Points::Y, py::const_),
                      py::overload_cast<const std::vector<double> &>(&las::Points::Y))
        .def_property("z", py::overload_cast<>(&las::Points::Z, py::const_),
                      py::overload_cast<const std::vector<double> &>(&las::Points::Z))
        .def_property("classification", py::overload_cast<>(&las::Points::Classification, py::const_),
                      py::overload_cast<const std::vector<uint8_t> &>(&las::Points::Classification))
        .def_property("point_source_id", py::overload_cast<>(&las::Points::PointSourceId, py::const_),
                      py::overload_cast<const std::vector<uint8_t> &>(&las::Points::PointSourceId))
        .def_property("red", py::overload_cast<>(&las::Points::Red, py::const_),
                      py::overload_cast<const std::vector<uint16_t> &>(&las::Points::Red))
        .def_property("green", py::overload_cast<>(&las::Points::Green, py::const_),
                      py::overload_cast<const std::vector<uint16_t> &>(&las::Points::Green))
        .def_property("blue", py::overload_cast<>(&las::Points::Blue, py::const_),
                      py::overload_cast<const std::vector<uint16_t> &>(&las::Points::Blue))
        .def_property_readonly("point_format_id", &las::Points::PointFormatId)
        .def_property_readonly("point_record_length", &las::Points::PointRecordLength)
        .def_property_readonly("eb_byte_size", &las::Points::EbByteSize)
        .def("AddPoint", &las::Points::AddPoint)
        .def("AddPoints", py::overload_cast<las::Points>(&las::Points::AddPoints))
        .def("AddPoints", py::overload_cast<std::vector<std::shared_ptr<las::Point>>>(&las::Points::AddPoints))
        .def("CreatePoint", &las::Points::CreatePoint)
        .def("ToPointFormat", &las::Points::ToPointFormat, py::arg("point_format_id"))
        .def("Within", &las::Points::Within, py::arg("box"))
        .def("GetWithin", &las::Points::GetWithin, py::arg("box"))
        .def("Pack", py::overload_cast<const Vector3 &,
                                         const Vector3 &>(&las::Points::Pack, py::const_))
        .def("Pack", py::overload_cast<const las::LasHeader &>(&las::Points::Pack, py::const_))
        .def("Unpack", py::overload_cast<const std::vector<char> &, const las::LasHeader &>(&las::Points::Unpack))
        .def("Unpack", py::overload_cast<const std::vector<char> &, const int8_t &, const uint16_t &, const Vector3 &,
                                         const Vector3 &>(&las::Points::Unpack))
        /// Bare bones interface
        .def("__getitem__",
             [wrap_i](const las::Points &s, DiffType i) {
                 i = wrap_i(i, s.Size());
                 return s[(SizeType)i];
             })
        .def("__setitem__",
             [wrap_i](las::Points &s, DiffType i, std::shared_ptr<las::Point> v) {
                 i = wrap_i(i, s.Size());
                 s[(SizeType)i] = std::move(v);
             })
        .def("__len__", &las::Points::Size)
        /// Optional sequence protocol operations
        .def(
            "__iter__", [](las::Points &s) { return py::make_iterator(s.begin(), s.end()); },
            py::keep_alive<0, 1>() /* Essential: keep object alive while iterator exists */)
        .def("__contains__",
             [](las::Points &s, std::shared_ptr<las::Point> v) { return std::find(s.begin(), s.end(), v) != s.end(); })
        /// Slicing protocol (optional)
        .def("__getitem__",
             [](const las::Points &s, const py::slice &slice) -> las::Points * {
                 size_t start = 0, stop = 0, step = 0, slicelength = 0;
                 if (!slice.compute(s.Size(), &start, &stop, &step, &slicelength))
                     throw py::error_already_set();

                 std::vector<std::shared_ptr<las::Point>> new_points;
                 new_points.reserve(slicelength);

                 for (size_t i = 0; i < slicelength; ++i)
                 {
                     new_points.push_back(s[start]);
                     start += step;
                 }

                 auto *seq = new las::Points(new_points);
                 return seq;
             })
        .def("__setitem__",
             [](las::Points &s, const py::slice &slice, const las::Points &value) {
                 size_t start = 0, stop = 0, step = 0, slicelength = 0;
                 if (!slice.compute(s.Size(), &start, &stop, &step, &slicelength))
                     throw py::error_already_set();

                 if (slicelength != value.Size())
                     throw std::runtime_error("Left and right hand size of slice assignment have different sizes!");

                 for (size_t i = 0; i < slicelength; ++i)
                 {
                     s[start] = value[i];
                     start += step;
                 }
             })
        .def("__str__", &las::Points::ToString)
        .def("__repr__", &las::Points::ToString);

    py::class_<FileReader>(m, "FileReader")
        .def(py::init<std::string &>())
        .def("Close", &FileReader::Close)
        .def_property_readonly("path", &FileReader::FilePath)
        .def("FindNode", &Reader::FindNode, py::arg("key"))
        .def_property_readonly("copc_config", &Reader::CopcConfig)
        .def("GetPointData", py::overload_cast<const Node &>(&Reader::GetPointData), py::arg("node"))
        .def("GetPointData", py::overload_cast<const VoxelKey &>(&Reader::GetPointData), py::arg("key"))
        .def("GetPoints", py::overload_cast<const Node &>(&Reader::GetPoints), py::arg("node"))
        .def("GetPoints", py::overload_cast<const VoxelKey &>(&Reader::GetPoints), py::arg("key"))
        .def("GetPointDataCompressed", py::overload_cast<const Node &>(&Reader::GetPointDataCompressed),
             py::arg("node"))
        .def("GetPointDataCompressed", py::overload_cast<const VoxelKey &>(&Reader::GetPointDataCompressed),
             py::arg("key"))
        .def("GetAllChildrenOfPage", &Reader::GetAllChildrenOfPage, py::arg("key"))
        .def("GetAllNodes", &Reader::GetAllNodes)
        .def("GetPageList", &Reader::GetPageList)
        .def("GetAllPoints", &Reader::GetAllPoints, py::arg("resolution") = 0)
        .def("GetNodesWithinBox", &Reader::GetNodesWithinBox, py::arg("box"), py::arg("resolution") = 0)
        .def("GetNodesIntersectBox", &Reader::GetNodesIntersectBox, py::arg("box"), py::arg("resolution") = 0)
        .def("GetPointsWithinBox", &Reader::GetPointsWithinBox, py::arg("box"), py::arg("resolution") = 0)
        .def("GetDepthAtResolution", &Reader::GetDepthAtResolution, py::arg("resolution"))
        .def("GetMaxDepth", &Reader::GetMaxDepth)
        .def("GetNodesAtResolution", &Reader::GetNodesAtResolution, py::arg("resolution"))
        .def("GetNodesWithinResolution", &Reader::GetNodesWithinResolution, py::arg("resolution"))
        .def("ValidateSpatialBounds", &Reader::ValidateSpatialBounds, py::arg("verbose") = false);

    py::class_<las::EbVlr>(m, "EbVlr").def(py::init<int>()).def_readwrite("items", &las::EbVlr::items);

    py::class_<FileWriter>(m, "FileWriter")
        .def(
            py::init<const std::string &, const CopcConfigWriter &, const std::optional<uint8_t> &,
                     const std::optional<Vector3> &, const std::optional<Vector3> &, const std::optional<std::string> &,
                     const std::optional<las::EbVlr> &, const std::optional<bool> &>(),
            py::arg("file_path"), py::arg("config"), py::arg("point_format_id") = py::none(),
            py::arg("scale") = py::none(), py::arg("offset") = py::none(), py::arg("wkt") = py::none(),
            py::arg("extra_bytes_vlr") = py::none(), py::arg("has_extended_stats") = py::none())
        .def_property_readonly("copc_config", &Writer::CopcConfig)
        .def_property_readonly("path", &FileWriter::FilePath)
        .def("FindNode", &Writer::FindNode)
        .def("Close", &FileWriter::Close)
        .def("AddNode", py::overload_cast<const VoxelKey &, const las::Points &, const VoxelKey &>(&Writer::AddNode),
             py::arg("key"), py::arg("points"), py::arg("page_key") = VoxelKey::RootKey())
        .def("AddNodeCompressed", &Writer::AddNodeCompressed, py::arg("key"), py::arg("compressed_data"),
             py::arg("point_count"), py::arg("page_key") = VoxelKey::RootKey())
        .def("AddNode",
             py::overload_cast<const VoxelKey &, std::vector<char> const &, const VoxelKey &>(&Writer::AddNode),
             py::arg("key"), py::arg("uncompressed_data"), py::arg("page_key") = VoxelKey::RootKey())
        .def("ChangeNodePage", &Writer::ChangeNodePage, py::arg("node_key"), py::arg("new_page_key"));

    py::class_<laz::LazFileReader>(m, "LazReader")
        .def(py::init<const std::string &>(), py::arg("file_path"))
        .def_property_readonly("laz_config", &laz::LazReader::LazConfig)
        .def_property_readonly("path", &laz::LazFileReader::FilePath)
        .def("GetPoints", py::overload_cast<>(&laz::LazReader::GetPoints));

    py::class_<laz::LazFileWriter>(m, "LazWriter")
        .def(py::init<const std::string &, const las::LazConfigWriter &>(), py::arg("file_path"), py::arg("config"))
        .def_property_readonly("laz_config", &laz::LazWriter::LazConfig)
        .def_property_readonly("point_count", &laz::LazWriter::PointCount)
        .def_property_readonly("chunk_count", &laz::LazWriter::ChunkCount)
        .def_property_readonly("path", &laz::LazFileWriter::FilePath)
        .def("Close", &laz::LazFileWriter::Close)
        .def("WritePoints", py::overload_cast<const las::Points &>(&laz::LazWriter::WritePoints), py::arg("points"))
        .def("WritePointsCompressed", &laz::LazWriter::WritePointsCompressed, py::arg("compressed_data"),
             py::arg("point_count"));

    m.def(
        "CompressBytes",
        py::overload_cast<const std::vector<char> &, const int8_t &, const uint16_t &>(&laz::Compressor::CompressBytes),
        py::arg("in"), py::arg("point_format_id"), py::arg("eb_byte_size"));
    m.def("CompressBytes",
          py::overload_cast<std::vector<char> &, const las::LasHeader &>(&laz::Compressor::CompressBytes));

    m.def("DecompressBytes",
          py::overload_cast<const std::vector<char> &, const las::LasHeader &, const int &>(
              &laz::Decompressor::DecompressBytes),
          py::arg("compressed_data"), py::arg("header"), py::arg("point_count"));
    m.def("DecompressBytes",
          py::overload_cast<const std::vector<char> &, const int8_t &, const uint16_t &, const int &>(
              &laz::Decompressor::DecompressBytes),
          py::arg("compressed_data"), py::arg("point_format_id"), py::arg("eb_byte_size"), py::arg("point_count"));

    py::class_<las::LasHeader, std::shared_ptr<las::LasHeader>>(m, "LasHeader")
        .def(py::init<>())
        .def_readwrite("file_source_id", &las::LasHeader::file_source_id)
        .def_readwrite("global_encoding", &las::LasHeader::global_encoding)
        .def_property("guid", py::overload_cast<>(&las::LasHeader::GUID, py::const_),
                      py::overload_cast<const std::string &>(&las::LasHeader::GUID))
        .def_property("system_identifier", py::overload_cast<>(&las::LasHeader::SystemIdentifier, py::const_),
                      py::overload_cast<const std::string &>(&las::LasHeader::SystemIdentifier))
        .def_property("generating_software", py::overload_cast<>(&las::LasHeader::GeneratingSoftware, py::const_),
                      py::overload_cast<const std::string &>(&las::LasHeader::GeneratingSoftware))
        .def_readwrite("creation_day", &las::LasHeader::creation_day)
        .def_readwrite("creation_year", &las::LasHeader::creation_year)
        .def_property_readonly("point_offset", &las::LasHeader::PointOffset)
        .def_property_readonly("vlr_count", &las::LasHeader::VlrCount)
        .def_property_readonly("point_format_id", &las::LasHeader::PointFormatId)
        .def_property_readonly("point_record_length", &las::LasHeader::PointRecordLength)
        .def_property_readonly("scale", &las::LasHeader::Scale)
        .def_property_readonly("offset", &las::LasHeader::Offset)
        .def_readwrite("max", &las::LasHeader::max)
        .def_readwrite("min", &las::LasHeader::min)
        .def_property_readonly("evlr_offset", &las::LasHeader::EvlrOffset)
        .def_property_readonly("evlr_count", &las::LasHeader::EvlrCount)
        .def_property_readonly("point_count", &las::LasHeader::PointCount)
        .def_readwrite("points_by_return", &las::LasHeader::points_by_return)
        .def("EbByteSize", &las::LasHeader::EbByteSize)
        .def("Span", &las::LasHeader::Span)
        .def("Bounds", &las::LasHeader::Bounds)
        .def("CheckAndUpdateBounds", &las::LasHeader::CheckAndUpdateBounds)
        .def("SetGpsTimeBit", &las::LasHeader::SetGpsTimeBit)
        .def("ApplyScale", &las::LasHeader::ApplyScale)
        .def("ApplyInverseScale", &las::LasHeader::ApplyInverseScale)
        .def("ApplyScaleX", &las::LasHeader::ApplyScaleX)
        .def("ApplyScaleY", &las::LasHeader::ApplyScaleY)
        .def("ApplyScaleZ", &las::LasHeader::ApplyScaleZ)
        .def("RemoveScaleX", &las::LasHeader::RemoveScaleX)
        .def("RemoveScaleY", &las::LasHeader::RemoveScaleY)
        .def("RemoveScaleZ", &las::LasHeader::RemoveScaleZ)
        .def("__str__", &las::LasHeader::ToString)
        .def("__repr__", &las::LasHeader::ToString)
        .def(py::pickle(
            [](const las::LasHeader &h) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(h.file_source_id, h.global_encoding, h.GUID(), h.SystemIdentifier(),
                                      h.GeneratingSoftware(), h.creation_day, h.creation_year, h.PointOffset(),
                                      h.VlrCount(), h.PointFormatId(), h.PointRecordLength(), h.Scale(), h.Offset(),
                                      h.max, h.min, h.EvlrOffset(), h.EvlrCount(), h.PointCount(), h.points_by_return,
                                      h.IsCopc());
            },
            [](const py::tuple &t) { // __setstate__
                if (t.size() != 20)
                    throw std::runtime_error("Invalid state!");

                /* Create a new C++ instance */
                las::LasHeader h(t[9].cast<int8_t>(), t[10].cast<uint16_t>(), t[7].cast<uint32_t>(),
                                 t[17].cast<uint64_t>(), t[8].cast<uint32_t>(), t[11].cast<Vector3>(),
                                 t[12].cast<Vector3>(), t[15].cast<uint64_t>(), t[16].cast<uint32_t>(),
                                 t[19].cast<bool>());
                h.file_source_id = t[0].cast<uint16_t>();
                h.global_encoding = t[1].cast<uint16_t>();
                h.GUID(t[2].cast<std::string>());
                h.SystemIdentifier(t[3].cast<std::string>());
                h.GeneratingSoftware(t[4].cast<std::string>());
                h.creation_day = t[5].cast<uint16_t>();
                h.creation_year = t[6].cast<uint16_t>();
                h.max = t[13].cast<Vector3>();
                h.min = t[14].cast<Vector3>();
                h.points_by_return = t[18].cast<std::array<uint64_t, 15>>();
                return h;
            }));

    py::class_<las::EbVlr::ebfield>(m, "EbField").def(py::init<>());

    py::class_<las::LazConfig, std::shared_ptr<las::LazConfig>>(m, "LazConfig")
        .def_property_readonly("las_header", &las::LazConfig::LasHeader)
        .def_property_readonly("extra_bytes_vlr", &las::LazConfig::ExtraBytesVlr)
        .def_property_readonly("wkt", &las::LazConfig::Wkt);

    py::class_<CopcConfig, std::shared_ptr<CopcConfig>>(m, "CopcConfig")
        .def_property_readonly("copc_info", &CopcConfig::CopcInfo)
        .def_property_readonly("copc_extents", &CopcConfig::CopcExtents)
        .def_property_readonly("las_header", &CopcConfig::LasHeader)
        .def_property_readonly("extra_bytes_vlr", &CopcConfig::ExtraBytesVlr)
        .def_property_readonly("wkt", &CopcConfig::Wkt);

    py::implicitly_convertible<CopcConfig, las::LazConfig>();

    py::class_<CopcConfigWriter, std::shared_ptr<CopcConfigWriter>>(m, "CopcConfigWriter")
        .def(
            py::init<const int8_t &, const Vector3 &, const Vector3 &, const std::string &, const las::EbVlr &, bool>(),
            py::arg("point_format_id"), py::arg("scale") = Vector3::DefaultScale(),
            py::arg("offset") = Vector3::DefaultOffset(), py::arg("wkt") = "",
            py::arg("extra_bytes_vlr") = las::EbVlr(0), py::arg("has_extended_stats") = false)
        .def(py::init<const CopcConfig &>())
        .def_property_readonly("las_header", py::overload_cast<>(&CopcConfigWriter::LasHeader))
        .def_property_readonly("copc_info", py::overload_cast<>(&CopcConfigWriter::CopcInfo))
        .def_property_readonly("copc_extents", py::overload_cast<>(&CopcConfigWriter::CopcExtents))
        .def_property_readonly("extra_bytes_vlr", &las::LazConfig::ExtraBytesVlr)
        .def_property_readonly("wkt", &las::LazConfig::Wkt);

    py::implicitly_convertible<CopcConfig, CopcConfigWriter>();

    py::class_<las::LazConfigWriter, std::shared_ptr<las::LazConfigWriter>>(m, "LazConfigWriter")
        .def(py::init<const int8_t &, const Vector3 &, const Vector3 &, const std::string &, const las::EbVlr &>(),
             py::arg("point_format_id"), py::arg("scale") = Vector3::DefaultScale(),
             py::arg("offset") = Vector3::DefaultOffset(), py::arg("wkt") = "",
             py::arg("extra_bytes_vlr") = las::EbVlr(0))
        .def(py::init<const las::LazConfig &>())
        .def(py::init<const CopcConfig &>())
        .def_property_readonly("las_header", py::overload_cast<>(&las::LazConfigWriter::LasHeader))
        .def_property_readonly("extra_bytes_vlr", &las::LazConfig::ExtraBytesVlr)
        .def_property_readonly("wkt", &las::LazConfig::Wkt);

    py::implicitly_convertible<las::LazConfig, las::LazConfigWriter>();
    py::implicitly_convertible<CopcConfig, las::LazConfigWriter>();

    py::class_<CopcInfo, std::shared_ptr<CopcInfo>>(m, "CopcInfo")
        .def(py::init<>())
        .def_readwrite("center_x", &CopcInfo::center_x)
        .def_readwrite("center_y", &CopcInfo::center_y)
        .def_readwrite("center_z", &CopcInfo::center_z)
        .def_readwrite("halfsize", &CopcInfo::halfsize)
        .def_readwrite("spacing", &CopcInfo::spacing)
        .def_readwrite("root_hier_offset", &CopcInfo::root_hier_offset)
        .def_readwrite("root_hier_size", &CopcInfo::root_hier_size)
        .def("__str__", &CopcInfo::ToString)
        .def("__repr__", &CopcInfo::ToString);

    py::class_<CopcExtent, std::shared_ptr<CopcExtent>>(m, "CopcExtent")
        .def(py::init<>())
        .def(py::init<double, double, double, double>(), py::arg("minimum"), py::arg("maximum"), py::arg("mean") = 0,
             py::arg("var") = 1)
        .def(py::init<const std::vector<double> &>(), py::arg("list"))
        .def_readwrite("minimum", &CopcExtent::minimum)
        .def_readwrite("maximum", &CopcExtent::maximum)
        .def_readwrite("mean", &CopcExtent::mean)
        .def_readwrite("var", &CopcExtent::var)
        .def(py::self == py::self)
        .def("__str__", &CopcExtent::ToString)
        .def("__repr__", &CopcExtent::ToString);

    py::implicitly_convertible<py::tuple, CopcExtent>();

    py::class_<CopcExtents, std::shared_ptr<CopcExtents>>(m, "CopcExtents")
        .def(py::init<int8_t, uint16_t>(), py::arg("point_format_id"), py::arg("num_eb_items") = 0)
        .def_property_readonly("point_format_id", &CopcExtents::PointFormatId)
        .def_property_readonly("has_extended_stats", &CopcExtents::HasExtendedStats)
        .def_property("x", py::overload_cast<>(&CopcExtents::X),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::X))
        .def_property("y", py::overload_cast<>(&CopcExtents::Y),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Y))
        .def_property("z", py::overload_cast<>(&CopcExtents::Z),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Z))
        .def_property("intensity", py::overload_cast<>(&CopcExtents::Intensity),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Intensity))
        .def_property("return_number", py::overload_cast<>(&CopcExtents::ReturnNumber),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::ReturnNumber))
        .def_property("number_of_returns", py::overload_cast<>(&CopcExtents::NumberOfReturns),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::NumberOfReturns))
        .def_property("scanner_channel", py::overload_cast<>(&CopcExtents::ScannerChannel),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::ScannerChannel))
        .def_property("scan_direction_flag", py::overload_cast<>(&CopcExtents::ScanDirectionFlag),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::ScanDirectionFlag))
        .def_property("edge_of_flight_line", py::overload_cast<>(&CopcExtents::EdgeOfFlightLine),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::EdgeOfFlightLine))
        .def_property("classification", py::overload_cast<>(&CopcExtents::Classification),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Classification))
        .def_property("user_data", py::overload_cast<>(&CopcExtents::UserData),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::UserData))
        .def_property("scan_angle", py::overload_cast<>(&CopcExtents::ScanAngle),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::ScanAngle))
        .def_property("point_source_id", py::overload_cast<>(&CopcExtents::PointSourceId),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::PointSourceId))
        .def_property("gps_time", py::overload_cast<>(&CopcExtents::GpsTime),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::GpsTime))
        .def_property("red", py::overload_cast<>(&CopcExtents::Red),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Red))
        .def_property("green", py::overload_cast<>(&CopcExtents::Green),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Green))
        .def_property("blue", py::overload_cast<>(&CopcExtents::Blue),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Blue))
        .def_property("nir", py::overload_cast<>(&CopcExtents::Nir),
                      py::overload_cast<std::shared_ptr<CopcExtent>>(&CopcExtents::Nir))
        .def_property("extra_bytes", py::overload_cast<>(&CopcExtents::ExtraBytes),
                      py::overload_cast<std::vector<std::shared_ptr<CopcExtent>>>(&CopcExtents::ExtraBytes))
        .def_property_readonly("extents", py::overload_cast<>(&CopcExtents::Extents))
        .def("__str__", &CopcExtents::ToString)
        .def("__repr__", &CopcExtents::ToString);
}
