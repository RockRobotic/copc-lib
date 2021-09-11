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
#include <copc-lib/las/point.hpp>
#include <copc-lib/las/points.hpp>

namespace py = pybind11;
using namespace copc;

PYBIND11_MODULE(copclib, m)
{

    py::class_<VoxelKey>(m, "VoxelKey")
        .def(py::init<>())
        .def(py::init<int32_t, int32_t, int32_t, int32_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
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

    // TODO[Leo]: Update functions after merging PR29
    py::class_<las::Points>(m, "Points")
        .def(py::init<const uint8_t &, const uint16_t &>())
        .def(py::init<const uint8_t &, const uint16_t &, const uint32_t &>())
        .def(py::init<const uint8_t &, const uint16_t &, const std::vector<las::Point> &>())
        .def("GetPoints", &las::Points::GetPoints)
        .def("AddPoint", &las::Points::AddPoint)
        .def("AddPoints", &las::Points::AddPoints)
        .def("PointFormatID", &las::Points::PointFormatID)
        .def("ToPointFormat", &las::Points::ToPointFormat)
        .def("PointRecordLength", &las::Points::PointRecordLength);

    py::class_<Node>(m, "Node").def(py::init<>()).def(py::init<Entry>());

    py::class_<FileReader>(m, "FileReader")
        .def(py::init<std::string &>())
        .def("GetPointData", static_cast<std::vector<char> (Reader::*)(const Node &)>(&Reader::GetPointData))
        .def("GetPointData", static_cast<std::vector<char> (Reader::*)(const VoxelKey &)>(&Reader::GetPointData))
        .def("GetPoints", static_cast<std::vector<las::Point> (Reader::*)(const Node &)>(&Reader::GetPoints))
        .def("GetPoints", static_cast<std::vector<las::Point> (Reader::*)(const VoxelKey &)>(&Reader::GetPoints))
        .def("GetPointDataCompressed",
             static_cast<std::vector<char> (Reader::*)(const Node &)>(&Reader::GetPointDataCompressed))
        .def("GetPointDataCompressed",
             static_cast<std::vector<char> (Reader::*)(const VoxelKey &)>(&Reader::GetPointDataCompressed))
        .def("GetAllChildren", static_cast<std::vector<Node> (Reader::*)(const VoxelKey &)>(&Reader::GetAllChildren))
        .def("GetAllChildren", static_cast<std::vector<Node> (Reader::*)()>(&Reader::GetAllChildren));

    py::class_<FileWriter>(m, "FileWriter")
        .def(py::init<std::string &, Writer::LasConfig const &, const int &, const std::string &>())
        .def("GetRootPage", &Writer::GetRootPage)
        .def("Close", &Writer::Close)
        .def("AddNode",
             static_cast<Node (Writer::*)(Page &, VoxelKey, std::vector<las::Point> const &)>(&Writer::AddNode))
        .def("AddNodeCompressed", &Writer::AddNodeCompressed)
        .def("AddNode", static_cast<Node (Writer::*)(Page &, VoxelKey, std::vector<char> const &)>(&Writer::AddNode))
        .def("AddSubPage", &Writer::AddSubPage);
}
