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

    //    py::class_<hierarchy::Entry>(m, "Entry")
    //        .def(py::init<>())
    //        .def(py::init<hierarchy::VoxelKey, int64_t, int32_t, int32_t>())
    //        .def("IsValid", &hierarchy::Entry::IsValid)
    //        .def("__str__", &hierarchy::Entry::ToString)
    //        .def("__repr__", &hierarchy::Entry::ToString);
    //
    //    py::class_<hierarchy::Node>(m, "Node")
    //        .def(py::init<>())
    //        .def(py::init<hierarchy::VoxelKey, int64_t, int32_t, int32_t,std::shared_ptr<io::Reader>>())
    //        .def("PackPoints", &hierarchy::Node::PackPoints)
    //        .def("GetPointData", &hierarchy::Node::GetPointData)
    //        .def("GetPoints", &hierarchy::Node::GetPoints);
    //
    //    py::class_<io::Reader>(m, "Reader")
    //        .def(py::init<std::istream &>());
    //
    //    py::class_<hierarchy::Page>(m, "Page")
    //        .def(py::init<>())
    //        .def(py::init<hierarchy::VoxelKey, int64_t, int32_t,std::shared_ptr<io::Reader>>())
    //        .def("InsertNode", &hierarchy::Page::InsertNode)
    //        .def("Load", &hierarchy::Page::Load)
    //        .def("Pack", &hierarchy::Page::Pack);
    //
    //    py::class_<Copc>(m, "Copc")
    //        .def(py::init<std::istream&>())
    //        .def(py::init<const std::string&>());
    //
    //    py::class_<hierarchy::Hierarchy>(m, "Hierarchy")
    //        .def(py::init<std::shared_ptr<io::Reader>>())
    //        .def("FindNode", &hierarchy::Hierarchy::FindNode)
    //        .def("NearestLoadedPage", &hierarchy::Hierarchy::NearestLoadedPage);
    //
    //    py::class_<las::LasFile>(m, "LasFile")
    //        .def(py::init<std::istream&>())
    //        .def("GetLasHeader", &las::LasFile::GetLasHeader);
    //
    //    py::class_<CopcFile>(m, "CopcFile")
    //        .def(py::init<std::istream&>())
    //        .def("GetWkt", &CopcFile::GetWkt)
    //        .def("SetWkt", &CopcFile::SetWkt)
    //        .def("GetCopcHeader", &CopcFile::GetCopcHeader);
    //
    //    py::class_<las::Point>(m, "Point")
    //        .def(py::init<const uint8_t &>())
    //        .def(py::init<const las::Point &>())
    //        .def("__str__", &las::Point::ToString)
    //        .def("__repr__", &las::Point::ToString);
    //        .def("Intensity", &las::Point::Intensity);
    //        .def("Y", &las::Point::Y)
    //        .def("Z", &las::Point::Z)
}
