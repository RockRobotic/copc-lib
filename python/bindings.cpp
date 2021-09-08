#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <copc-lib/copc/file.hpp>
#include <copc-lib/hierarchy/entry.hpp>
#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
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
        .def("Bisect", &VoxelKey::Bisect)
        .def("GetParent", &VoxelKey::GetParent)
        .def("GetParents", &VoxelKey::GetParents)
        .def("__str__", &VoxelKey::ToString)
        .def("__repr__", &VoxelKey::ToString)
        .def(py::hash(py::self));

    py::class_<las::Point>(m, "Point")
        .def(py::init<const uint8_t &, const uint16_t &>())
        .def(py::init<const las::Point &>())
        //        .def("Intensity", &las::Point::Intensity)
        .def("__str__", &las::Point::ToString)
        .def("__repr__", &las::Point::ToString);

    py::class_<las::Points>(m, "Points")
        .def(py::init<const uint8_t &, const uint16_t &>())
        .def(py::init<const uint8_t &, const uint16_t &, const uint32_t &>())
        .def(py::init<const uint8_t &, const uint16_t &, const std::vector<las::Point> &>())
        .def("GetPoints", &las::Points::GetPoints)
        .def("PointFormatID", &las::Points::PointFormatID)
        .def("PointRecordLength", &las::Points::PointRecordLength);
}
