import copclib
import pytest


def test_reader():
    # Given a valid file path
    reader = copclib.FileReader("../build/test/data/autzen-classified.copc.laz")

    # GetLasHeader Test
    copc = reader.GetCopcHeader()
    assert copc.span == 0
    assert copc.root_hier_offset == 93169718
    assert copc.root_hier_size == 8896
    assert copc.laz_vlr_offset == 643
    assert copc.laz_vlr_size == 58
    assert copc.wkt_vlr_offset == 755
    assert copc.wkt_vlr_size == 993
    assert copc.eb_vlr_offset == 1802
    assert copc.eb_vlr_size == 384

    # GetLasHeader Test
    header = reader.GetLasHeader()
    assert header.header_size == 375
    assert header.point_format_id == 3
    assert header.point_count == 10653336

    # WKT Test
    wkt = reader.GetWkt()
    assert len(wkt) > 0
    assert wkt.rfind("COMPD_CS[", 0) == 0


def test_find_key():
    # Given a valid file path
    reader = copclib.FileReader("../build/test/data/autzen-classified.copc.laz")

    key = copclib.VoxelKey.BaseKey()
    hier_entry = reader.FindNode(key)

    assert hier_entry.IsValid() == True
    assert hier_entry.key == key
    assert hier_entry.point_count == 61022

    key = copclib.VoxelKey(5, 9, 7, 0)
    hier_entry = reader.FindNode(key)

    assert hier_entry.IsValid() == True
    assert hier_entry.key == key
    assert hier_entry.point_count == 12019


def test_get_extra_byte_vlrs():
    reader = copclib.FileReader("../build/test/data/autzen-classified.copc.laz")

    eb_vlr = reader.GetExtraByteVlr()
    assert len(eb_vlr.items) == 2

    assert eb_vlr.items[0].data_type == 1
    assert eb_vlr.items[0].name == "FIELD_0"

    assert eb_vlr.items[1].data_type == 1
    assert eb_vlr.items[1].name == "FIELD_1"


def test_get_all_children():
    reader = copclib.FileReader("../build/test/data/autzen-classified.copc.laz")

    # Get root key
    nodes = reader.GetAllChildren()
    assert len(nodes) == 278

    # Get an invalid key
    nodes = reader.GetAllChildren(copclib.VoxelKey.InvalidKey())
    assert len(nodes) == 0

    # Get an existing key
    nodes = reader.GetAllChildren(copclib.VoxelKey(5, 9, 7, 0))
    assert len(nodes) == 1
    assert nodes[0].IsValid()
    assert nodes[0].key == copclib.VoxelKey(5, 9, 7, 0)

    # Get a non-existing key
    nodes = reader.GetAllChildren(copclib.VoxelKey(20, 20, 20, 20))
    assert len(nodes) == 0


def test_point_error_handling():
    reader = copclib.FileReader("../build/test/data/autzen-classified.copc.laz")

    invalid_node = copclib.Node()
    with pytest.raises(RuntimeError):
        reader.GetPointData(invalid_node)
    valid_node = reader.FindNode(copclib.VoxelKey(5, 9, 7, 0))
    reader.GetPointData(valid_node)

    assert len(reader.GetPointData(invalid_node.key)) == 0
    assert not len(reader.GetPointData(valid_node.key)) == 0

    with pytest.raises(RuntimeError):
        reader.GetPoints(invalid_node)
    reader.GetPoints(valid_node)

    assert len(reader.GetPoints(invalid_node.key).Get()) == 0
    assert not len(reader.GetPoints(valid_node.key).Get()) == 0

    with pytest.raises(RuntimeError):
        reader.GetPointDataCompressed(invalid_node)
    reader.GetPointDataCompressed(valid_node)
