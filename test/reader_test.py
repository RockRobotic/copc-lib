import copclib as copc
import pytest
from sys import float_info


def test_reader():
    # Given a valid file path
    reader = copc.FileReader("autzen-classified.copc.laz")

    # GetLasHeader Test
    copc_header = reader.GetCopcHeader()
    assert copc_header.span == 128
    assert copc_header.root_hier_offset == 93169718
    assert copc_header.root_hier_size == 8896
    assert copc_header.laz_vlr_offset == 643
    assert copc_header.laz_vlr_size == 58
    assert copc_header.wkt_vlr_offset == 755
    assert copc_header.wkt_vlr_size == 993
    assert copc_header.eb_vlr_offset == 1802
    assert copc_header.eb_vlr_size == 384

    # GetLasHeader Test
    header = reader.GetLasHeader()
    assert header.header_size == 375
    assert header.point_format_id == 3
    assert header.point_count == 10653336
    assert header.point_record_length == 36
    assert header.num_extra_bytes == 2

    # WKT Test
    wkt = reader.GetWkt()
    assert len(wkt) > 0
    assert wkt.startswith("COMPD_CS[")


def test_find_key():
    # Given a valid file path
    reader = copc.FileReader("autzen-classified.copc.laz")

    key = copc.VoxelKey.BaseKey()
    hier_entry = reader.FindNode(key)

    assert hier_entry.IsValid() == True
    assert hier_entry.key == key
    assert hier_entry.point_count == 61022

    key = copc.VoxelKey(5, 9, 7, 0)
    hier_entry = reader.FindNode(key)

    assert hier_entry.IsValid() == True
    assert hier_entry.key == key
    assert hier_entry.point_count == 12019


def test_get_all_children():
    reader = copc.FileReader("autzen-classified.copc.laz")

    # Get root key
    nodes = reader.GetAllChildren()
    assert len(nodes) == 278

    # Get an invalid key
    nodes = reader.GetAllChildren(copc.VoxelKey.InvalidKey())
    assert len(nodes) == 0

    # Get an existing key
    nodes = reader.GetAllChildren(copc.VoxelKey(5, 9, 7, 0))
    assert len(nodes) == 1
    assert nodes[0].IsValid()
    assert nodes[0].key == copc.VoxelKey(5, 9, 7, 0)

    # Get a non-existing key
    nodes = reader.GetAllChildren(copc.VoxelKey(20, 20, 20, 20))
    assert len(nodes) == 0


# TODO[Leo]: Make this test optional

# def test_get_all_points():
#     reader = copc.FileReader("autzen-classified.copc.laz")
#     assert len(reader.GetAllPoints()) == reader.GetLasHeader().point_count


def test_point_error_handling():
    reader = copc.FileReader("autzen-classified.copc.laz")

    invalid_node = copc.Node()
    with pytest.raises(RuntimeError):
        reader.GetPointData(node=invalid_node)
    valid_node = reader.FindNode(copc.VoxelKey(5, 9, 7, 0))
    reader.GetPointData(valid_node)

    assert len(reader.GetPointData(key=invalid_node.key)) == 0
    assert len(reader.GetPointData(valid_node.key)) != 0

    with pytest.raises(RuntimeError):
        reader.GetPoints(invalid_node)
    reader.GetPoints(valid_node)

    assert len(reader.GetPoints(invalid_node.key)) == 0
    assert len(reader.GetPoints(valid_node.key)) != 0

    with pytest.raises(RuntimeError):
        reader.GetPointDataCompressed(invalid_node)
    reader.GetPointDataCompressed(valid_node)


def test_spatial_query_functions():

    reader = copc.FileReader("autzen-classified.copc.laz")

    # Make horizontal 2D box of [200,200] roughly in the middle of the point cloud.
    middle = (reader.GetLasHeader().max + reader.GetLasHeader().min) / 2
    middle_box = (middle.x - 200, middle.y - 200, middle.x + 200, middle.y + 200)

    # GetNodesWithinBox

    ## Check that no nodes fit in a zero-sized box
    subset_nodes = reader.GetNodesWithinBox(copc.Box.ZeroBox())
    assert len(subset_nodes) == 0

    ## Check that all nodes fit in a max-sized box
    subset_nodes = reader.GetNodesWithinBox(copc.Box.MaxBox())
    all_nodes = reader.GetAllChildren()
    assert len(subset_nodes) == len(all_nodes)

    # GetNodesIntersectBox

    subset_nodes = reader.GetNodesIntersectBox(middle_box)
    assert len(subset_nodes) == 13

    # GetPointsWithinBox

    ## Check that no points fit in a zero-sized box
    subset_points = reader.GetPointsWithinBox(copc.Box.ZeroBox())
    assert len(subset_points) == 0

    # TODO[Leo]: Make this test optional
    ## Check that all points fit in a box sized from header
    # header = reader.GetLasHeader()
    # subset_points = reader.GetPointsWithinBox(
    #     copc.Box(
    #         math.floor(header.min.x),
    #         math.floor(header.min.y),
    #         math.floor(header.min.z),
    #         math.ceil(header.max.x),
    #         math.ceil(header.max.y),
    #         math.ceil(header.max.z),
    #     )
    # )
    # assert len(subset_points) == header.point_count

    subset_points = reader.GetPointsWithinBox(middle_box)
    assert len(subset_points) == 91178

    # GetDepthAtResolution
    assert reader.GetDepthAtResolution(3) == 4
    assert reader.GetDepthAtResolution(0) == 5
    assert reader.GetDepthAtResolution(float_info.min) == 5
    assert reader.GetDepthAtResolution(float_info.max) == 0

    # GetNodesAtResolution
    subset_nodes = reader.GetNodesAtResolution(3)
    assert len(subset_nodes) == 192
    for node in reader.GetNodesAtResolution(0):
        assert node.key.d == 5

    # GetNodesWithinResolution
    subset_nodes = reader.GetNodesWithinResolution(3)
    assert len(subset_nodes) == 257
    assert len(reader.GetNodesWithinResolution(0)) == len(reader.GetAllChildren())
