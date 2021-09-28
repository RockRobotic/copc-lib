# import copclib as copc
# import pytest
#
# def test_copc_stats():
#
#     point_format_id = 6
#     num_extra_bytes = 5
#
#     # Constructor {
#     # Empty constructor
#
#     stats = copc.copc.CopcStats(point_format_id, num_extra_bytes)
#     assert stats.point_format_id == point_format_id
#     assert stats.extra_bytes.size() == num_extra_bytes
#
#     # Filled Constructor
#
#     extents = [copc.CopcExtent(copc.CopcStats.NumberOfExtents(point_format_id,num_extra_bytes))
#     stats = copc.CopcStats(extents, point_format_id, num_extra_bytes)
#     assert stats.point_format_id == point_format_id
#     assert stats.extra_bytes.size() == num_extra_bytes
#     assert stats.x.minimum == 1
#     assert stats.x.maximum == 1
#     # Point format checks
#     with pytest.raises(RuntimeError):
#         copc.CopcStats(5)
#         copc.CopcStats(9)
#         copc.CopcStats(std::vector<las::CopcExtent>(3),point_format_id,num_extra_bytes)
#
#     # ToCopcExtents
#
#     copc.CopcStats stats{point_format_id,num_extra_bytes}
#
#     extents = stats.ToCopcExtents()
#     assert extents.size() == copc.CopcStats::NumberOfExtents(point_format_id, num_extra_bytes)
#
#     # FromCopcExtents
#     copc.CopcStats stats{point_format_id,num_extra_bytes}
#
#     extents = std::vector<las::CopcExtent>(copc.CopcStats::NumberOfExtents(point_format_id,num_extra_bytes),{1,1})
#
#     stats.FromCopcExtents(extents)
#
#     assert stats.x.minimum == 1
#     assert stats.x.maximum == 1
#
#     REQUIRE_THROWS(stats.FromCopcExtents(std::vector<las::CopcExtent>(3)))
#
