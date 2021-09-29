#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <copc-lib/copc/extents.hpp>
#include <copc-lib/geometry/box.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/file.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>
#include <copc-lib/laz/compressor.hpp>
#include <copc-lib/laz/decompressor.hpp>

namespace py = pybind11;
using namespace copc;

PYBIND11_MAKE_OPAQUE(std::vector<char>);

PYBIND11_MODULE(copclib, m)
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
        .def(py::init<int32_t, int32_t, int32_t, int32_t>(), py::arg("d"), py::arg("x"), py::arg("y"), py::arg("z"))
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("d", &VoxelKey::d)
        .def_readwrite("x", &VoxelKey::x)
        .def_readwrite("y", &VoxelKey::y)
        .def_readwrite("z", &VoxelKey::z)
        .def("IsValid", &VoxelKey::IsValid)
        .def("BaseKey", &VoxelKey::BaseKey)
        .def("InvalidKey", &VoxelKey::InvalidKey)
        .def("Bisect", &VoxelKey::Bisect)
        .def("GetParent", &VoxelKey::GetParent)
        .def("GetParents", &VoxelKey::GetParents, py::arg("include_current"))
        .def("ChildOf", &VoxelKey::ChildOf, py::arg("parent_key"))
        .def("Intersects", &VoxelKey::Intersects)
        .def("Contains", py::overload_cast<const Box &, const las::LasHeader &>(&VoxelKey::Contains, py::const_))
        .def("Contains", py::overload_cast<const Vector3 &, const las::LasHeader &>(&VoxelKey::Contains, py::const_))
        .def("Within", &VoxelKey::Within)
        .def("__str__", &VoxelKey::ToString)
        .def("__repr__", &VoxelKey::ToString);
    py::implicitly_convertible<py::tuple, VoxelKey>();

    py::class_<Box>(m, "Box")
        .def(py::init<>())
        .def(py::init<const double &, const double &, const double &, const double &, const double &, const double &>(),
             py::arg("x_min"), py::arg("y_min"), py::arg("z_min"), py::arg("x_max"), py::arg("y_max"), py::arg("z_max"))
        .def(py::init<const double &, const double &, const double &, const double &>(), py::arg("x_min"),
             py::arg("y_min"), py::arg("x_max"), py::arg("y_max"))
        .def(py::init<const std::vector<double> &>(), py::arg("list"))
        .def(py::init<const VoxelKey &, const las::LasHeader &>(), py::arg("key"), py::arg("las_header"))
        .def_readwrite("x_min", &Box::x_min)
        .def_readwrite("y_min", &Box::y_min)
        .def_readwrite("z_min", &Box::z_min)
        .def_readwrite("x_max", &Box::x_max)
        .def_readwrite("y_max", &Box::y_max)
        .def_readwrite("z_max", &Box::z_max)
        .def_static("MaxBox", &Box::MaxBox)
        .def_static("ZeroBox", &Box::ZeroBox)
        .def("Intersects", &Box::Intersects)
        .def("Contains", py::overload_cast<const Box &>(&Box::Contains, py::const_))
        .def("Contains", py::overload_cast<const Vector3 &>(&Box::Contains, py::const_))
        .def("Within", &Box::Within)
        .def("__str__", &Box::ToString)
        .def("__repr__", &Box::ToString);

    py::implicitly_convertible<py::list, Box>();
    py::implicitly_convertible<py::tuple, Box>();

    py::class_<Node>(m, "Node")
        .def(py::init<>())
        .def(py::init<Entry>())
        .def_readwrite("point_count", &Node::point_count)
        .def_readwrite("key", &Node::key)
        .def_readwrite("offset", &Node::offset)
        .def_readwrite("byte_size", &Node::byte_size)
        .def("IsValid", &Node::IsValid)
        .def("IsPage", &Node::IsPage)
        .def("__str__", &Node::ToString)
        .def("__repr__", &Node::ToString);

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
        .def(py::init<const double &, const double &, const double &>())
        .def(py::init<const std::vector<double> &>())
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        .def_static("DefaultScale", &Vector3::DefaultScale)
        .def_static("DefaultOffset", &Vector3::DefaultOffset)
        .def(py::self == py::self)
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
        .def_property("X", py::overload_cast<>(&las::Point::X, py::const_),
                      py::overload_cast<const double &>(&las::Point::X))
        .def_property("Y", py::overload_cast<>(&las::Point::Y, py::const_),
                      py::overload_cast<const double &>(&las::Point::Y))
        .def_property("Z", py::overload_cast<>(&las::Point::Z, py::const_),
                      py::overload_cast<const double &>(&las::Point::Z))
        .def_property("UnscaledX", py::overload_cast<>(&las::Point::UnscaledX, py::const_),
                      py::overload_cast<const int32_t &>(&las::Point::UnscaledX))
        .def_property("UnscaledY", py::overload_cast<>(&las::Point::UnscaledY, py::const_),
                      py::overload_cast<const int32_t &>(&las::Point::UnscaledY))
        .def_property("UnscaledZ", py::overload_cast<>(&las::Point::UnscaledZ, py::const_),
                      py::overload_cast<const int32_t &>(&las::Point::UnscaledZ))
        .def_property("Intensity", py::overload_cast<>(&las::Point::Intensity, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Intensity))
        .def_property("ReturnsScanDirEofBitFields",
                      py::overload_cast<>(&las::Point::ReturnsScanDirEofBitFields, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ReturnsScanDirEofBitFields))
        .def_property("ExtendedReturnsBitFields",
                      py::overload_cast<>(&las::Point::ExtendedReturnsBitFields, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ExtendedReturnsBitFields))
        .def_property("ReturnNumber", py::overload_cast<>(&las::Point::ReturnNumber, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ReturnNumber))
        .def_property("NumberOfReturns", py::overload_cast<>(&las::Point::NumberOfReturns, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::NumberOfReturns))
        .def_property("ExtendedFlagsBitFields", py::overload_cast<>(&las::Point::ExtendedFlagsBitFields, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ExtendedFlagsBitFields))
        .def_property("Synthetic", py::overload_cast<>(&las::Point::Synthetic, py::const_),
                      py::overload_cast<const bool &>(&las::Point::Synthetic))
        .def_property("KeyPoint", py::overload_cast<>(&las::Point::KeyPoint, py::const_),
                      py::overload_cast<const bool &>(&las::Point::KeyPoint))
        .def_property("Withheld", py::overload_cast<>(&las::Point::Withheld, py::const_),
                      py::overload_cast<const bool &>(&las::Point::Withheld))
        .def_property("Overlap", py::overload_cast<>(&las::Point::Overlap, py::const_),
                      py::overload_cast<const bool &>(&las::Point::Overlap))
        .def_property("ScannerChannel", py::overload_cast<>(&las::Point::ScannerChannel, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ScannerChannel))
        .def_property("ScanDirectionFlag", py::overload_cast<>(&las::Point::ScanDirectionFlag, py::const_),
                      py::overload_cast<const bool &>(&las::Point::ScanDirectionFlag))
        .def_property("EdgeOfFlightLineFlag", py::overload_cast<>(&las::Point::EdgeOfFlightLineFlag, py::const_),
                      py::overload_cast<const bool &>(&las::Point::EdgeOfFlightLineFlag))
        .def_property("ClassificationBitFields", py::overload_cast<>(&las::Point::ClassificationBitFields, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::ClassificationBitFields))
        .def_property("Classification", py::overload_cast<>(&las::Point::Classification, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::Classification))
        .def_property("ScanAngleRank", py::overload_cast<>(&las::Point::ScanAngleRank, py::const_),
                      py::overload_cast<const int8_t &>(&las::Point::ScanAngleRank))
        .def_property("ExtendedScanAngle", py::overload_cast<>(&las::Point::ExtendedScanAngle, py::const_),
                      py::overload_cast<const int16_t &>(&las::Point::ExtendedScanAngle))
        .def_property("ScanAngle", py::overload_cast<>(&las::Point::ScanAngle, py::const_),
                      py::overload_cast<const float &>(&las::Point::ScanAngle))
        .def_property("UserData", py::overload_cast<>(&las::Point::UserData, py::const_),
                      py::overload_cast<const uint8_t &>(&las::Point::UserData))
        .def_property("PointSourceID", py::overload_cast<>(&las::Point::PointSourceID, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::PointSourceID))
        .def_property("RGB", nullptr, py::overload_cast<const std::vector<uint16_t> &>(&las::Point::RGB))
        .def_property("Red", py::overload_cast<>(&las::Point::Red, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Red))
        .def_property("Green", py::overload_cast<>(&las::Point::Green, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Green))
        .def_property("Blue", py::overload_cast<>(&las::Point::Blue, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::Blue))
        .def_property("GPSTime", py::overload_cast<>(&las::Point::GPSTime, py::const_),
                      py::overload_cast<const double &>(&las::Point::GPSTime))
        .def_property("NIR", py::overload_cast<>(&las::Point::NIR, py::const_),
                      py::overload_cast<const uint16_t &>(&las::Point::NIR))

        .def_property_readonly("HasExtendedPoint", &las::Point::HasExtendedPoint)
        .def_property_readonly("HasGPSTime", &las::Point::HasGPSTime)
        .def_property_readonly("HasRGB", &las::Point::HasRGB)
        .def_property_readonly("HasRGB", &las::Point::HasRGB)
        .def_property_readonly("HasNIR", &las::Point::HasNIR)

        .def("ToPointFormat", &las::Point::ToPointFormat, py::arg("point_format_id"))

        .def_property_readonly("PointFormatID", &las::Point::PointFormatID)
        .def_property_readonly("PointRecordLength", &las::Point::PointRecordLength)
        .def_property_readonly("NumExtraBytes", &las::Point::NumExtraBytes)

        .def_property("ExtraBytes", py::overload_cast<>(&las::Point::ExtraBytes, py::const_),
                      py::overload_cast<const std::vector<uint8_t> &>(&las::Point::ExtraBytes))

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
        .def(py::init<const uint8_t &, const Vector3 &, const Vector3 &, const uint16_t &>(),
             py::arg("point_format_id"), py::arg("scale"), py::arg("offset"), py::arg("num_extra_bytes") = 0)
        .def(py::init<const std::vector<std::shared_ptr<las::Point>> &>(), py::arg("points"))
        .def(py::init<const las::LasHeader &>())
        .def_property("X", py::overload_cast<>(&las::Points::X, py::const_),
                      py::overload_cast<const std::vector<double> &>(&las::Points::X))
        .def_property("Y", py::overload_cast<>(&las::Points::Y, py::const_),
                      py::overload_cast<const std::vector<double> &>(&las::Points::Y))
        .def_property("Z", py::overload_cast<>(&las::Points::Z, py::const_),
                      py::overload_cast<const std::vector<double> &>(&las::Points::Z))
        .def_property_readonly("PointFormatID", &las::Points::PointFormatID)
        .def_property_readonly("PointRecordLength", &las::Points::PointRecordLength)
        .def_property_readonly("NumExtraBytes", &las::Points::NumExtraBytes)
        .def("AddPoint", &las::Points::AddPoint)
        .def("AddPoints", py::overload_cast<las::Points>(&las::Points::AddPoints))
        .def("AddPoints", py::overload_cast<std::vector<std::shared_ptr<las::Point>>>(&las::Points::AddPoints))
        .def("CreatePoint", &las::Points::CreatePoint)
        .def("ToPointFormat", &las::Points::ToPointFormat, py::arg("point_format_id"))
        .def("Within", &las::Points::Within, py::arg("box"))
        .def("Pack", py::overload_cast<>(&las::Points::Pack))
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
                 s[(SizeType)i] = v;
             })
        // todo?
        //.def(
        //    "__delitem__",
        //    [wrap_i](las::Points &s, size_t i) {
        //        i = wrap_i(i, s.Size());
        //    },
        //    "Delete the list elements at index ``i``")
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

    py::class_<CopcExtent>(m, "CopcExtent")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<const std::vector<double> &>())
        .def_readwrite("minimum", &CopcExtent::minimum)
        .def_readwrite("maximum", &CopcExtent::maximum);

    py::implicitly_convertible<py::list, CopcExtent>();
    py::implicitly_convertible<py::tuple, CopcExtent>();

    py::class_<FileReader>(m, "FileReader")
        .def(py::init<std::string &>())
        .def("Close", &FileReader::Close)
        .def("FindNode", &Reader::FindNode, py::arg("key"))
        .def("GetWkt", &Reader::GetWkt)
        .def("GetCopcInfo", &Reader::GetCopcInfo)
        .def("GetCopcExtents", &Reader::GetCopcExtents)
        .def("GetLasHeader", &Reader::GetLasHeader)
        .def("GetExtraByteVlr", &Reader::GetExtraByteVlr)
        .def("GetPointData", py::overload_cast<const Node &>(&Reader::GetPointData), py::arg("node"))
        .def("GetPointData", py::overload_cast<const VoxelKey &>(&Reader::GetPointData), py::arg("key"))
        .def("GetPoints", py::overload_cast<const Node &>(&Reader::GetPoints), py::arg("node"))
        .def("GetPoints", py::overload_cast<const VoxelKey &>(&Reader::GetPoints), py::arg("key"))
        .def("GetPointDataCompressed", py::overload_cast<const Node &>(&Reader::GetPointDataCompressed),
             py::arg("node"))
        .def("GetPointDataCompressed", py::overload_cast<const VoxelKey &>(&Reader::GetPointDataCompressed),
             py::arg("key"))
        .def("GetAllChildren", py::overload_cast<const VoxelKey &>(&Reader::GetAllChildren), py::arg("key"))
        .def("GetAllChildren", py::overload_cast<>(&Reader::GetAllChildren))
        .def("GetAllPoints", &Reader::GetAllPoints)
        .def("GetNodesWithinBox", &Reader::GetNodesWithinBox)
        .def("GetNodesIntersectBox", &Reader::GetNodesIntersectBox)
        .def("GetPointsWithinBox", &Reader::GetPointsWithinBox);

    py::class_<FileWriter>(m, "FileWriter")
        .def(py::init<const std::string &, Writer::LasConfig const &, const int &, const std::string &>(),
             py::arg("file_path"), py::arg("config"), py::arg("span") = 0, py::arg("wkt") = "")
        .def("FindNode", &Writer::FindNode)
        .def("GetWkt", &Writer::GetWkt)
        .def("GetCopcInfo", &Writer::GetCopcInfo)
        .def("GetCopcExtents", &Writer::GetCopcExtents)
        .def("SetCopcExtents", &Writer::SetCopcExtents)
        .def("GetLasHeader", &Writer::GetLasHeader)
        .def("GetExtraByteVlr", &Writer::GetExtraByteVlr)
        .def("GetRootPage", &Writer::GetRootPage)
        .def("Close", &FileWriter::Close)
        .def("AddNode", py::overload_cast<Page &, const VoxelKey &, las::Points &>(&Writer::AddNode), py::arg("page"),
             py::arg("key"), py::arg("points"))
        .def("AddNodeCompressed", &Writer::AddNodeCompressed)
        .def("AddNode", py::overload_cast<Page &, const VoxelKey &, std::vector<char> const &>(&Writer::AddNode),
             py::arg("page"), py::arg("key"), py::arg("uncompressed_data"))
        .def("AddSubPage", &Writer::AddSubPage, py::arg("parent_page"), py::arg("key"))
        .def("SetMin", &Writer::SetMin)
        .def("SetMax", &Writer::SetMax)
        .def("SetPointsByReturn", &Writer::SetPointsByReturn);

    m.def("CompressBytes",
          py::overload_cast<std::vector<char> &, const int8_t &, const uint16_t &>(&laz::Compressor::CompressBytes),
          py::arg("in"), py::arg("point_format_id"), py::arg("num_extra_bytes"));
    m.def("CompressBytes",
          py::overload_cast<std::vector<char> &, const las::LasHeader &>(&laz::Compressor::CompressBytes));

    m.def("DecompressBytes",
          py::overload_cast<const std::vector<char> &, const las::LasHeader &, const int &>(
              &laz::Decompressor::DecompressBytes),
          py::arg("compressed_data"), py::arg("header"), py::arg("point_count"));
    m.def("DecompressBytes",
          py::overload_cast<const std::vector<char> &, const int8_t &, const uint16_t &, const int &>(
              &laz::Decompressor::DecompressBytes),
          py::arg("compressed_data"), py::arg("point_format_id"), py::arg("num_extra_bytes"), py::arg("point_count"));

    py::class_<las::LasHeader>(m, "LasHeader")
        .def(py::init<>())
        .def_property_readonly("num_extra_bytes", &las::LasHeader::NumExtraBytes)
        .def_readwrite("file_source_id", &las::LasHeader::file_source_id)
        .def_readwrite("global_encoding", &las::LasHeader::global_encoding)
        .def_property("guid", py::overload_cast<>(&las::LasHeader::GUID, py::const_),
                      py::overload_cast<const std::string &>(&las::LasHeader::GUID))
        .def_readwrite("version_minor", &las::LasHeader::version_minor)
        .def_readwrite("version_major", &las::LasHeader::version_major)
        .def_property("system_identifier", py::overload_cast<>(&las::LasHeader::SystemIdentifier, py::const_),
                      py::overload_cast<const std::string &>(&las::LasHeader::SystemIdentifier))
        .def_property("generating_software", py::overload_cast<>(&las::LasHeader::GeneratingSoftware, py::const_),
                      py::overload_cast<const std::string &>(&las::LasHeader::GeneratingSoftware))
        .def_readwrite("creation_day", &las::LasHeader::creation_day)
        .def_readwrite("creation_year", &las::LasHeader::creation_year)
        .def_readwrite("header_size", &las::LasHeader::header_size)
        .def_readwrite("point_offset", &las::LasHeader::point_offset)
        .def_readwrite("vlr_count", &las::LasHeader::vlr_count)
        .def_readwrite("point_format_id", &las::LasHeader::point_format_id)
        .def_readwrite("point_record_length", &las::LasHeader::point_record_length)
        .def_readwrite("point_count", &las::LasHeader::point_count)
        .def_readwrite("points_by_return", &las::LasHeader::points_by_return)
        .def_readwrite("scale", &las::LasHeader::scale)
        .def_readwrite("offset", &las::LasHeader::offset)
        .def_readwrite("max", &las::LasHeader::max)
        .def_readwrite("min", &las::LasHeader::min)
        .def("GetSpan", &las::LasHeader::GetSpan)
        .def("GetBounds", &las::LasHeader::GetBounds)
        .def_readwrite("wave_offset", &las::LasHeader::wave_offset)
        .def_readwrite("evlr_offset", &las::LasHeader::evlr_offset)
        .def_readwrite("evlr_count", &las::LasHeader::evlr_count)
        .def_readwrite("point_count_14", &las::LasHeader::point_count)
        .def_readwrite("points_by_return_14", &las::LasHeader::points_by_return_14)
        .def(py::pickle(
            [](const las::LasHeader &h) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(h.file_source_id, h.global_encoding, h.GUID(), h.version_major, h.version_minor,
                                      h.SystemIdentifier(), h.GeneratingSoftware(), h.creation_day, h.creation_year,
                                      h.header_size, h.point_offset, h.vlr_count, h.point_format_id,
                                      h.point_record_length, h.point_count, h.points_by_return, h.scale, h.offset,
                                      h.max, h.min, h.wave_offset, h.evlr_offset, h.evlr_count, h.point_count_14,
                                      h.points_by_return_14);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 25)
                    throw std::runtime_error("Invalid state!");

                /* Create a new C++ instance */
                las::LasHeader h;
                h.file_source_id = t[0].cast<uint16_t>();
                h.global_encoding = t[1].cast<uint16_t>();
                h.GUID(t[2].cast<std::string>());
                h.version_major = t[3].cast<uint8_t>();
                h.version_minor = t[4].cast<uint8_t>();
                h.SystemIdentifier(t[5].cast<std::string>());
                h.GeneratingSoftware(t[6].cast<std::string>());
                h.creation_day = t[7].cast<uint16_t>();
                h.creation_year = t[8].cast<uint16_t>();
                h.header_size = t[9].cast<uint16_t>();
                h.point_offset = t[10].cast<uint32_t>();
                h.vlr_count = t[11].cast<uint32_t>();
                h.point_format_id = t[12].cast<int8_t>();
                h.point_record_length = t[13].cast<uint16_t>();
                h.point_count = t[14].cast<uint32_t>();
                h.points_by_return = t[15].cast<std::array<uint32_t, 5>>();
                h.scale = t[16].cast<Vector3>();
                h.offset = t[17].cast<Vector3>();
                h.max = t[18].cast<Vector3>();
                h.min = t[19].cast<Vector3>();
                h.wave_offset = t[20].cast<uint64_t>();
                h.evlr_offset = t[21].cast<uint64_t>();
                h.evlr_count = t[22].cast<uint32_t>();
                h.point_count_14 = t[23].cast<uint64_t>();
                h.points_by_return_14 = t[24].cast<std::array<uint64_t, 15>>();
                return h;
            }));

    py::class_<Writer::LasConfig>(m, "LasConfig")
        .def(py::init<const int8_t &, const Vector3 &, const Vector3 &>(), py::arg("point_format_id"),
             py::arg("scale") = Vector3::DefaultScale(), py::arg("offset") = Vector3::DefaultOffset())
        .def(py::init<const las::LasHeader &, const las::EbVlr &>())
        .def_readwrite("file_source_id", &Writer::LasConfig::file_source_id)
        .def_readwrite("global_encoding", &Writer::LasConfig::global_encoding)
        .def_readwrite("creation_day", &Writer::LasConfig::creation_day)
        .def_readwrite("creation_year", &Writer::LasConfig::creation_year)
        .def_readwrite("point_format_id", &Writer::LasConfig::point_format_id)
        .def_readwrite("extra_bytes", &Writer::LasConfig::extra_bytes)
        .def_readwrite("scale", &Writer::LasConfig::scale)
        .def_readwrite("offset", &Writer::LasConfig::offset)
        .def_readwrite("max", &Writer::LasConfig::max)
        .def_readwrite("min", &Writer::LasConfig::min)
        .def_readwrite("points_by_return_14", &Writer::LasConfig::points_by_return_14)
        .def_property("guid", py::overload_cast<>(&Writer::LasConfig::GUID, py::const_),
                      py::overload_cast<const std::string &>(&Writer::LasConfig::GUID))
        .def_property("system_identifier", py::overload_cast<>(&Writer::LasConfig::SystemIdentifier, py::const_),
                      py::overload_cast<const std::string &>(&Writer::LasConfig::SystemIdentifier))
        .def_property("generating_software", py::overload_cast<>(&Writer::LasConfig::GeneratingSoftware, py::const_),
                      py::overload_cast<const std::string &>(&Writer::LasConfig::GeneratingSoftware))
        .def("NumExtraBytes", &Writer::LasConfig::NumExtraBytes);

    py::class_<las::CopcInfoVlr>(m, "CopcVlr")
        .def_readwrite("span", &las::CopcInfoVlr::span)
        .def_readwrite("root_hier_offset", &las::CopcInfoVlr::root_hier_offset)
        .def_readwrite("root_hier_size", &las::CopcInfoVlr::root_hier_size)
        .def_readwrite("laz_vlr_offset", &las::CopcInfoVlr::laz_vlr_offset)
        .def_readwrite("laz_vlr_size", &las::CopcInfoVlr::laz_vlr_size)
        .def_readwrite("wkt_vlr_offset", &las::CopcInfoVlr::wkt_vlr_offset)
        .def_readwrite("wkt_vlr_size", &las::CopcInfoVlr::wkt_vlr_size)
        .def_readwrite("eb_vlr_offset", &las::CopcInfoVlr::eb_vlr_offset)
        .def_readwrite("eb_vlr_size", &las::CopcInfoVlr::eb_vlr_size);

    py::class_<las::EbVlr>(m, "EbVlr").def(py::init<int>());

    py::class_<CopcExtents>(m, "CopcExtents")
        .def(py::init<int8_t, uint16_t>(), py::arg("point_format_id"), py::arg("num_extra_bytes") = 0)
        .def_static("NumberOfExtents", &CopcExtents::NumberOfExtents)
        .def_readwrite("point_format_id", &CopcExtents::point_format_id)
        .def_readwrite("x", &CopcExtents::x)
        .def_readwrite("y", &CopcExtents::y)
        .def_readwrite("z", &CopcExtents::z)
        .def_readwrite("intensity", &CopcExtents::intensity)
        .def_readwrite("return_number", &CopcExtents::return_number)
        .def_readwrite("number_of_returns", &CopcExtents::number_of_returns)
        .def_readwrite("scanner_channel", &CopcExtents::scanner_channel)
        .def_readwrite("scan_direction_flag", &CopcExtents::scan_direction_flag)
        .def_readwrite("edge_of_flight_line", &CopcExtents::edge_of_flight_line)
        .def_readwrite("classification", &CopcExtents::classification)
        .def_readwrite("user_data", &CopcExtents::user_data)
        .def_readwrite("scan_angle", &CopcExtents::scan_angle)
        .def_readwrite("point_source_id", &CopcExtents::point_source_id)
        .def_readwrite("gps_time", &CopcExtents::gps_time)
        .def_readwrite("red", &CopcExtents::red)
        .def_readwrite("green", &CopcExtents::green)
        .def_readwrite("blue", &CopcExtents::blue)
        .def_readwrite("nir", &CopcExtents::nir)
        .def_readwrite("extra_bytes", &CopcExtents::extra_bytes)
        .def("GetCopcExtents", &CopcExtents::GetCopcExtents)
        .def("SetCopcExtents", &CopcExtents::SetCopcExtents);
}
