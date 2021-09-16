#include <iostream>
#include <vector>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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
        .def("__repr__", &VoxelKey::ToString);

    py::class_<Vector3>(m, "Vector3")
        .def(py::init<const double &, const double &, const double &>())
        .def(py::init<const std::vector<double> &>())
        .def_readwrite("x", &Vector3::x)
        .def_readwrite("y", &Vector3::y)
        .def_readwrite("z", &Vector3::z)
        .def(py::self == py::self);

    py::implicitly_convertible<py::list, Vector3>();

    py::class_<las::Point>(m, "Point")
        .def(py::init<const uint8_t &, const uint16_t &, const Vector3 &, const Vector3 &>(),
             py::arg("point_format_id"), py::arg("num_extra_bytes") = 0, py::arg("scale") = Vector3(0.01, 0.01, 0.01),
             py::arg("offset") = Vector3(0, 0, 0))
        .def(py::init<const las::LasHeader &>())
        .def(py::init<const las::Point &>())
        //        .def_property("X", py::overload_cast<>(&las::Point::X, py::const_),
        //                      py::overload_cast<const double &>(&las::Point::X))
        //        .def_property("Y", py::overload_cast<>(&las::Point::Y, py::const_),
        //                      py::overload_cast<const double &>(&las::Point::Y))
        //        .def_property("Z", py::overload_cast<>(&las::Point::Z, py::const_),
        //                      py::overload_cast<const double &>(&las::Point::Z))
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

        .def("Pack", &las::Point::Pack)
        .def("Unpack", &las::Point::Unpack)
        .def("ToPointFormat", &las::Point::ToPointFormat)

        .def_property_readonly("PointFormatID", &las::Point::PointFormatID)
        .def_property_readonly("PointRecordLength", &las::Point::PointRecordLength)
        .def_property_readonly("NumExtraBytes", &las::Point::NumExtraBytes)

        .def_property("ExtraBytes", py::overload_cast<>(&las::Point::ExtraBytes, py::const_),
                      py::overload_cast<const std::vector<uint8_t> &>(&las::Point::ExtraBytes))

        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__str__", &las::Point::ToString)
        .def("__repr__", &las::Point::ToString);

    py::class_<las::Points>(m, "Points")
        .def(py::init<const uint8_t &, const Vector3 &, const Vector3 &, const uint16_t &>(),
             py::arg("point_format_id"), py::arg("scale"), py::arg("offset"), py::arg("num_extra_bytes") = 0)
        .def(py::init<const std::vector<las::Point> &>())
        .def(py::init<const las::LasHeader &>())
        .def_property_readonly("PointFormatID", &las::Points::PointFormatID)
        .def_property_readonly("PointRecordLength", &las::Points::PointRecordLength)
        .def_property_readonly("NumExtraBytes", &las::Points::NumExtraBytes)
        .def("Get", static_cast<std::vector<las::Point> (las::Points::*)()>(&las::Points::Get))
        .def("Get", static_cast<las::Point (las::Points::*)(const size_t &)>(&las::Points::Get))
        .def_property_readonly("Size", &las::Points::Size)
        .def("Reserve", &las::Points::Reserve)
        .def("AddPoint", &las::Points::AddPoint)
        .def("AddPoints", static_cast<void (las::Points::*)(las::Points)>(&las::Points::AddPoints))
        .def("AddPoints", static_cast<void (las::Points::*)(std::vector<las::Point>)>(&las::Points::AddPoints))
        .def("CreatePoint", &las::Points::CreatePoint)
        .def("ToPointFormat", &las::Points::ToPointFormat)
        .def("Pack", static_cast<std::vector<char> (las::Points::*)()>(&las::Points::Pack))
        .def("Pack", static_cast<void (las::Points::*)(std::ostream &)>(&las::Points::Pack))
        .def("Unpack", py::overload_cast<const std::vector<char> &, const int8_t &, const uint16_t &, const Vector3 &,
                                         const Vector3 &>(&las::Points::Unpack))
        .def("Unpack", py::overload_cast<const std::vector<char> &, const las::LasHeader &>(&las::Points::Unpack))
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

    m.def("CompressBytes",
          py::overload_cast<std::vector<char> &, const int8_t &, const uint16_t &>(&laz::Compressor::CompressBytes));
    m.def("CompressBytes",
          py::overload_cast<std::vector<char> &, const las::LasHeader &>(&laz::Compressor::CompressBytes));

    m.def("DecompressBytes", py::overload_cast<const std::vector<char> &, const las::LasHeader &, const int &>(
                                 &laz::Decompressor::DecompressBytes));

    py::class_<Writer::LasConfig>(m, "LasConfig")
        .def(py::init<const int8_t &, const Vector3 &, const Vector3 &>(), py::arg("point_format_id"),
             py::arg("scale") = Vector3(0.01, 0.01, 0.01), py::arg("offset") = Vector3(0, 0, 0))
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
                      py::overload_cast<const std::string &>(&Writer::LasConfig::GeneratingSoftware));

    py::class_<FileWriter>(m, "FileWriter")
        .def(py::init<const std::string &, FileWriter::LasConfig const &, const int &, const std::string &>(),
             py::arg("file_path"), py::arg("config"), py::arg("span") = 0, py::arg("wkt") = "")
        .def("FindNode", &FileReader::FindNode)
        .def("GetWkt", &FileWriter::GetWkt)
        .def("GetCopcHeader", &FileWriter::GetCopcHeader)
        .def("GetLasHeader", &FileWriter::GetLasHeader)
        .def("GetExtraByteVlr", &FileWriter::GetExtraByteVlr)
        .def(py::init<std::string &, FileWriter::LasConfig const &, const int &, const std::string &>())
        .def("GetRootPage", &FileWriter::GetRootPage)
        .def("Close", &FileWriter::Close)
        .def("AddNode", static_cast<Node (FileWriter::*)(Page &, const VoxelKey &, las::Points &)>(&Writer::AddNode))
        .def("AddNodeCompressed", &FileWriter::AddNodeCompressed)
        .def("AddNode",
             static_cast<Node (FileWriter::*)(Page &, const VoxelKey &, std::vector<char> const &)>(&Writer::AddNode))
        .def("AddSubPage", &FileWriter::AddSubPage);

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

    py::class_<las::LasHeader>(m, "LasHeader")
        .def_readwrite("magic", &las::LasHeader::magic)
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
        .def_readwrite("wave_offset", &las::LasHeader::wave_offset)
        .def_readwrite("evlr_offset", &las::LasHeader::evlr_offset)
        .def_readwrite("evlr_count", &las::LasHeader::evlr_count)
        .def_readwrite("points_by_return_14", &las::LasHeader::points_by_return_14);

    //TODO[Leo]: Make our own EbVlr
    py::class_<las::EbVlr>(m, "EbVlr").def(py::init<int>()).def_readwrite("items", &las::EbVlr::items);

    //TODO[Leo]: Make our own ebfields
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
