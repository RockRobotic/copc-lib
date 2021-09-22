import copclib as copc
import pytest


def test_reader():
    # Given a valid file path
    reader = copc.FileReader("data/autzen-classified.copc.laz")

    # GetLasHeader Test
    copc_header = reader.GetCopcHeader()
    assert copc_header.span == 0
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
    reader = copc.FileReader("data/autzen-classified.copc.laz")

    key = copc.VoxelKey().BaseKey()
    hier_entry = reader.FindNode(key)

    assert hier_entry.IsValid() == True
    assert hier_entry.key == key
    assert hier_entry.point_count == 61022

    key = (5, 9, 7, 0)  # Test implicit conversion of key to tuple
    hier_entry = reader.FindNode(key)

    assert hier_entry.IsValid() == True
    assert hier_entry.key == key
    assert hier_entry.point_count == 12019


def test_get_all_children():
    reader = copc.FileReader("data/autzen-classified.copc.laz")

    # Get root key
    nodes = reader.GetAllChildren()
    assert len(nodes) == 278

    # Get an invalid key
    nodes = reader.GetAllChildren(copc.VoxelKey().InvalidKey())
    assert len(nodes) == 0

    # Get an existing key
    nodes = reader.GetAllChildren((5, 9, 7, 0))
    assert len(nodes) == 1
    assert nodes[0].IsValid()
    assert nodes[0].key == [5, 9, 7, 0]  # Test implicit conversion of key to list

    # Get a non-existing key
    nodes = reader.GetAllChildren((20, 20, 20, 20))
    assert len(nodes) == 0


def test_point_error_handling():
    reader = copc.FileReader("data/autzen-classified.copc.laz")

    invalid_node = copc.Node()
    with pytest.raises(RuntimeError):
        reader.GetPointData(node=invalid_node)
    valid_node = reader.FindNode((5, 9, 7, 0))
    reader.GetPointData(valid_node)

    assert len(reader.GetPointData(key=invalid_node.key)) == 0
    assert len(reader.GetPointData(valid_node.key)) != 0

    with pytest.raises(RuntimeError):
        reader.GetPoints(invalid_node)
    reader.GetPoints(valid_node)

    assert len(reader.GetPoints(invalid_node.key).Get()) == 0
    assert len(reader.GetPoints(valid_node.key).Get()) != 0

    with pytest.raises(RuntimeError):
        reader.GetPointDataCompressed(invalid_node)
    reader.GetPointDataCompressed(valid_node)
