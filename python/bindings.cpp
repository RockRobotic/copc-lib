#include <iostream>
#include <utility>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "lazperf/vlr.hpp"
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/file.hpp>
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>
#include <copc-lib/laz/compressor.hpp>

namespace py = pybind11;
using namespace copc;

PYBIND11_MODULE(copclib, m)
{

    py::class_<VoxelKey>(m, "VoxelKey")
        .def(py::init<>())
        .def(py::init<int32_t, int32_t, int32_t, int32_t>())
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
        .def("GetParents", &VoxelKey::GetParents)
        .def("ChildOf", &VoxelKey::ChildOf)
        .def("__str__", &VoxelKey::ToString)
        .def("__repr__", &VoxelKey::ToString)
        .def(py::hash(py::self));

    // TODO[Leo]: Update getters and setters to pybind template
    py::class_<las::Point>(m, "Point")
        .def(py::init<const uint8_t &, const uint16_t &>(), py::arg("point_format_id"), py::arg("num_extra_bytes") = 0)
        .def(py::init<const las::Point &>())
        .def("X", static_cast<int32_t (las::Point::*)() const>(&las::Point::X))
        .def("X", static_cast<void (las::Point::*)(const int32_t &)>(&las::Point::X))
        .def("Y", static_cast<int32_t (las::Point::*)() const>(&las::Point::Y))
        .def("Y", static_cast<void (las::Point::*)(const int32_t &)>(&las::Point::Y))
        .def("Z", static_cast<int32_t (las::Point::*)() const>(&las::Point::Z))
        .def("Z", static_cast<void (las::Point::*)(const int32_t &)>(&las::Point::Z))
        .def("Intensity", static_cast<uint16_t (las::Point::*)() const>(&las::Point::Intensity))
        .def("Intensity", static_cast<void (las::Point::*)(const uint16_t &)>(&las::Point::Intensity))
        .def("ReturnsScanDirEofBitFields",
             static_cast<uint8_t (las::Point::*)() const>(&las::Point::ReturnsScanDirEofBitFields))
        .def("ReturnsScanDirEofBitFields",
             static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::ReturnsScanDirEofBitFields))
        .def("ExtendedReturnsBitFields",
             static_cast<uint8_t (las::Point::*)() const>(&las::Point::ExtendedReturnsBitFields))
        .def("ExtendedReturnsBitFields",
             static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::ExtendedReturnsBitFields))
        .def("ReturnNumber", static_cast<uint8_t (las::Point::*)() const>(&las::Point::ReturnNumber))
        .def("ReturnNumber", static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::ReturnNumber))
        .def("NumberOfReturns", static_cast<uint8_t (las::Point::*)() const>(&las::Point::NumberOfReturns))
        .def("NumberOfReturns", static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::NumberOfReturns))
        .def("ExtendedFlagsBitFields",
             static_cast<uint8_t (las::Point::*)() const>(&las::Point::ExtendedFlagsBitFields))
        .def("ExtendedFlagsBitFields",
             static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::ExtendedFlagsBitFields))
        .def("Synthetic", static_cast<bool (las::Point::*)() const>(&las::Point::Synthetic))
        .def("Synthetic", static_cast<void (las::Point::*)(const bool &)>(&las::Point::Synthetic))
        .def("KeyPoint", static_cast<bool (las::Point::*)() const>(&las::Point::KeyPoint))
        .def("KeyPoint", static_cast<void (las::Point::*)(const bool &)>(&las::Point::KeyPoint))
        .def("Withheld", static_cast<bool (las::Point::*)() const>(&las::Point::Withheld))
        .def("Withheld", static_cast<void (las::Point::*)(const bool &)>(&las::Point::Withheld))
        .def("Overlap", static_cast<bool (las::Point::*)() const>(&las::Point::Overlap))
        .def("Overlap", static_cast<void (las::Point::*)(const bool &)>(&las::Point::Overlap))
        .def("ScannerChannel", static_cast<uint8_t (las::Point::*)() const>(&las::Point::ScannerChannel))
        .def("ScannerChannel", static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::ScannerChannel))
        .def("ScanDirectionFlag", static_cast<bool (las::Point::*)() const>(&las::Point::ScanDirectionFlag))
        .def("ScanDirectionFlag", static_cast<void (las::Point::*)(const bool &)>(&las::Point::ScanDirectionFlag))
        .def("EdgeOfFlightLineFlag", static_cast<bool (las::Point::*)() const>(&las::Point::EdgeOfFlightLineFlag))
        .def("EdgeOfFlightLineFlag", static_cast<void (las::Point::*)(const bool &)>(&las::Point::EdgeOfFlightLineFlag))
        .def("ClassificationBitFields",
             static_cast<uint8_t (las::Point::*)() const>(&las::Point::ClassificationBitFields))
        .def("ClassificationBitFields",
             static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::ClassificationBitFields))
        .def("Classification", static_cast<uint8_t (las::Point::*)() const>(&las::Point::Classification))
        .def("Classification", static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::Classification))
        .def("ScanAngleRank", static_cast<int8_t (las::Point::*)() const>(&las::Point::ScanAngleRank))
        .def("ScanAngleRank", static_cast<void (las::Point::*)(const int8_t &)>(&las::Point::ScanAngleRank))
        .def("ExtendedScanAngle", static_cast<int16_t (las::Point::*)() const>(&las::Point::ExtendedScanAngle))
        .def("ExtendedScanAngle", static_cast<void (las::Point::*)(const int16_t &)>(&las::Point::ExtendedScanAngle))
        .def("ScanAngle", static_cast<float (las::Point::*)() const>(&las::Point::ScanAngle))
        .def("ScanAngle", static_cast<void (las::Point::*)(const float &)>(&las::Point::ScanAngle))
        .def("UserData", static_cast<uint8_t (las::Point::*)() const>(&las::Point::UserData))
        .def("UserData", static_cast<void (las::Point::*)(const uint8_t &)>(&las::Point::UserData))
        .def("PointSourceID", static_cast<uint16_t (las::Point::*)() const>(&las::Point::PointSourceID))
        .def("PointSourceID", static_cast<void (las::Point::*)(const uint16_t &)>(&las::Point::PointSourceID))
        .def("RGB",
             static_cast<void (las::Point::*)(const uint16_t &, const uint16_t &, const uint16_t &)>(&las::Point::RGB))
        .def("Red", static_cast<uint16_t (las::Point::*)() const>(&las::Point::Red))
        .def("Red", static_cast<void (las::Point::*)(const uint16_t &)>(&las::Point::Red))
        .def("Green", static_cast<uint16_t (las::Point::*)() const>(&las::Point::Green))
        .def("Green", static_cast<void (las::Point::*)(const uint16_t &)>(&las::Point::Green))
        .def("Blue", static_cast<uint16_t (las::Point::*)() const>(&las::Point::Blue))
        .def("Blue", static_cast<void (las::Point::*)(const uint16_t &)>(&las::Point::Blue))
        .def("GPSTime", static_cast<double (las::Point::*)() const>(&las::Point::GPSTime))
        .def("GPSTime", static_cast<void (las::Point::*)(const double &)>(&las::Point::GPSTime))
        .def("NIR", static_cast<uint16_t (las::Point::*)() const>(&las::Point::NIR))
        .def("NIR", static_cast<void (las::Point::*)(const uint16_t &)>(&las::Point::NIR))

        .def("HasExtendedPoint", &las::Point::HasExtendedPoint)
        .def("HasGPSTime", &las::Point::HasGPSTime)
        .def("HasRGB", &las::Point::HasRGB)
        .def("HasNIR", &las::Point::HasNIR)

        .def("Pack", &las::Point::Pack)
        .def("Unpack", &las::Point::Unpack)
        .def("ToPointFormat", &las::Point::ToPointFormat)
        .def("BaseByteSize", &las::Point::BaseByteSize)

        .def("PointFormatID", &las::Point::PointFormatID)
        .def("PointRecordLength", &las::Point::PointRecordLength)
        .def("NumExtraBytes", &las::Point::NumExtraBytes)

        .def("ExtraBytes", static_cast<std::vector<uint8_t> (las::Point::*)() const>(&las::Point::ExtraBytes))
        .def("ExtraBytes", static_cast<void (las::Point::*)(const std::vector<uint8_t> &)>(&las::Point::ExtraBytes))

        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__str__", &las::Point::ToString)
        .def("__repr__", &las::Point::ToString);

    py::class_<las::Points>(m, "Points")
        .def(py::init<const uint8_t &, const uint16_t &>(), py::arg("point_format_id"), py::arg("num_extra_bytes") = 0)
        .def(py::init<const std::vector<las::Point> &>())
        .def("PointFormatID", &las::Points::PointFormatID)
        .def("PointRecordLength", &las::Points::PointRecordLength)
        .def("NumExtraBytes", &las::Points::NumExtraBytes)
        .def("Get", static_cast<std::vector<las::Point> (las::Points::*)()>(&las::Points::Get))
        .def("Get", static_cast<las::Point (las::Points::*)(const size_t &)>(&las::Points::Get))
        .def("Size", &las::Points::Size)
        .def("Reserve", &las::Points::Reserve)
        .def("AddPoint", &las::Points::AddPoint)
        .def("AddPoints", static_cast<void (las::Points::*)(las::Points)>(&las::Points::AddPoints))
        .def("AddPoints", static_cast<void (las::Points::*)(std::vector<las::Point>)>(&las::Points::AddPoints))
        .def("NewPoint", &las::Points::NewPoint)
        .def("ToPointFormat", &las::Points::ToPointFormat)
        .def("Pack", static_cast<std::vector<char> (las::Points::*)()>(&las::Points::Pack))
        .def("Pack", static_cast<void (las::Points::*)(std::ostream &)>(&las::Points::Pack))
        .def("Unpack", &las::Points::Unpack)
        .def("__str__", &las::Points::ToString)
        .def("__repr__", &las::Points::ToString);

    py::class_<FileReader>(m, "FileReader")
        .def(py::init<std::string &>())
        .def("FindNode", &FileReader::FindNode)
        .def("GetWkt", &FileReader::GetWkt)
        .def("GetCopcHeader", &FileReader::GetCopcHeader)
        .def("GetLasHeader", &FileReader::GetLasHeader)
        .def("GetExtraByteVlr", &FileReader::GetExtraByteVlr)
        .def("GetPointData", static_cast<std::vector<char> (Reader::*)(const Node &)>(&Reader::GetPointData))
        .def("GetPointData", static_cast<std::vector<char> (Reader::*)(const VoxelKey &)>(&Reader::GetPointData))
        .def("GetPoints", static_cast<las::Points (Reader::*)(const Node &)>(&Reader::GetPoints))
        .def("GetPoints", static_cast<las::Points (Reader::*)(const VoxelKey &)>(&Reader::GetPoints))
        .def("GetPointDataCompressed",
             static_cast<std::vector<char> (Reader::*)(const Node &)>(&Reader::GetPointDataCompressed))
        .def("GetPointDataCompressed",
             static_cast<std::vector<char> (Reader::*)(const VoxelKey &)>(&Reader::GetPointDataCompressed))
        .def("GetAllChildren", static_cast<std::vector<Node> (Reader::*)(const VoxelKey &)>(&Reader::GetAllChildren))
        .def("GetAllChildren", static_cast<std::vector<Node> (Reader::*)()>(&Reader::GetAllChildren));

    //    py::class_<laz::Compressor>(m, "LazCompressor")

    py::class_<copc::vector3>(m, "copc::vector3")
        .def(py::init<>())
        .def(py::init<const double &, const double &, const double &>())
        .def_readwrite("x", &copc::vector3::x)
        .def_readwrite("y", &copc::vector3::y)
        .def_readwrite("z", &copc::vector3::z);

    // TODO[Leo]: Finish commented attributes
    py::class_<Writer::LasConfig>(m, "LasConfig")
        .def(py::init<const int8_t &, const copc::vector3 &, const copc::vector3 &>(), py::arg("point_format_id"),
             py::arg("scale") = copc::vector3(0.01, 0.01, 0.01), py::arg("offset") = copc::vector3(0, 0, 0))
        .def(py::init<const las::LasHeader &, const las::EbVlr &>())
        .def_readwrite("file_source_id", &Writer::LasConfig::file_source_id)
        .def_readwrite("global_encoding", &Writer::LasConfig::global_encoding)
        //        .def_readwrite("guid", &Writer::LasConfig::guid)
        //        .def_readwrite("system_identifier", &Writer::LasConfig::system_identifier)
        //        .def_readwrite("generating_software", &Writer::LasConfig::generating_software)
        .def_readwrite("creation", &Writer::LasConfig::creation)
        .def_readwrite("point_format_id", &Writer::LasConfig::point_format_id)
        .def_readwrite("extra_bytes", &Writer::LasConfig::extra_bytes)
        .def_readwrite("scale", &Writer::LasConfig::scale)
        .def_readwrite("offset", &Writer::LasConfig::offset)
        .def_readwrite("max", &Writer::LasConfig::max)
        .def_readwrite("min", &Writer::LasConfig::min);
    //        .def_readwrite("points_by_return_14", &Writer::LasConfig::points_by_return_14);

    py::class_<FileWriter>(m, "FileWriter")
        .def(py::init<const std::string &, Writer::LasConfig const &, const int &, const std::string &>(),
             py::arg("file_path"), py::arg("config"), py::arg("span") = 0, py::arg("wkt") = "")
        .def("FindNode", &FileReader::FindNode)
        .def("GetWkt", &FileWriter::GetWkt)
        .def("GetCopcHeader", &FileWriter::GetCopcHeader)
        .def("GetLasHeader", &FileWriter::GetLasHeader)
        .def("GetExtraByteVlr", &FileWriter::GetExtraByteVlr)
        .def(py::init<std::string &, Writer::LasConfig const &, const int &, const std::string &>())
        .def("GetRootPage", &Writer::GetRootPage)
        .def("Close", &Writer::Close)
        .def("AddNode", static_cast<Node (Writer::*)(Page &, const VoxelKey &, las::Points &)>(&Writer::AddNode))
        .def("AddNodeCompressed", &Writer::AddNodeCompressed)
        .def("AddNode",
             static_cast<Node (Writer::*)(Page &, const VoxelKey &, std::vector<char> const &)>(&Writer::AddNode))
        .def("AddSubPage", &Writer::AddSubPage);

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

    py::class_<las::CopcVlr>(m, "CopcVlr")
        .def_readwrite("span", &las::CopcVlr::span)
        .def_readwrite("root_hier_offset", &las::CopcVlr::root_hier_offset)
        .def_readwrite("root_hier_size", &las::CopcVlr::root_hier_size)
        .def_readwrite("laz_vlr_offset", &las::CopcVlr::laz_vlr_offset)
        .def_readwrite("laz_vlr_size", &las::CopcVlr::laz_vlr_size)
        .def_readwrite("wkt_vlr_offset", &las::CopcVlr::wkt_vlr_offset)
        .def_readwrite("wkt_vlr_size", &las::CopcVlr::wkt_vlr_size)
        .def_readwrite("eb_vlr_offset", &las::CopcVlr::eb_vlr_offset)
        .def_readwrite("eb_vlr_size", &las::CopcVlr::eb_vlr_size);

    // TODO[Leo]: Update this after making our own lazperf headers
    py::class_<las::LasHeader>(m, "LasHeader")
        //        .def_readwrite("magic", &las::LasHeader::magic)
        .def_readwrite("file_source_id", &las::LasHeader::file_source_id)
        .def_readwrite("global_encoding", &las::LasHeader::global_encoding)
        //        .def_readwrite("guid", &las::LasHeader::guid)
        .def_readwrite("version", &las::LasHeader::version)
        //        .def_readwrite("system_identifier", &las::LasHeader::system_identifier)
        //        .def_readwrite("generating_software", &las::LasHeader::generating_software)
        .def_readwrite("scale", &las::LasHeader::scale)
        .def_readwrite("offset", &las::LasHeader::offset)
        .def_readwrite("header_size", &las::LasHeader::header_size)
        .def_readwrite("point_format_id", &las::LasHeader::point_format_id)
        .def_readwrite("point_record_length", &las::LasHeader::point_record_length)
        .def_readwrite("point_count", &las::LasHeader::point_count);

    py::class_<las::EbVlr>(m, "EbVlr").def(py::init<int>()).def_readwrite("items", &las::EbVlr::items);

    py::class_<las::EbVlr::ebfield>(m, "EbField")
        //    .def_readwrite("reserved",&las::EbVlr::ebfield::reserved)
        .def_readwrite("data_type", &las::EbVlr::ebfield::data_type)
        .def_readwrite("options", &las::EbVlr::ebfield::options)
        .def_readwrite("name", &las::EbVlr::ebfield::name)
        //    .def_readwrite("unused",&las::EbVlr::ebfield::unused)
        //    .def_readwrite("no_data",&las::EbVlr::ebfield::no_data)
        //    .def_readwrite("minval",&las::EbVlr::ebfield::minval)
        //    .def_readwrite("maxval",&las::EbVlr::ebfield::maxval)
        //    .def_readwrite("scale",&las::EbVlr::ebfield::scale)
        //    .def_readwrite("offset",&las::EbVlr::ebfield::offset)
        .def_readwrite("description", &las::EbVlr::ebfield::description)
        .def(py::self == py::self);
}
