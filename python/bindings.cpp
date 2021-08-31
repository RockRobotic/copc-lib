#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <copc-lib/hierarchy/key.hpp>
#include <copc-lib/las/point.hpp>

namespace py = pybind11;
using namespace copc;

PYBIND11_MODULE(copclib, m) {

    py::class_<hierarchy::VoxelKey>(m, "VoxelKey")
        .def(py::init<>())
        .def(py::init<int32_t, int32_t, int32_t, int32_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("IsValid", &hierarchy::VoxelKey::IsValid)
        .def("Bisect", &hierarchy::VoxelKey::Bisect)
        .def("GetParent", &hierarchy::VoxelKey::GetParent)
        .def("GetParents", &hierarchy::VoxelKey::GetParents)
        .def("__str__", &hierarchy::VoxelKey::ToString)
        .def("__repr__", &hierarchy::VoxelKey::ToString)
        .def(py::hash(py::self));

}

