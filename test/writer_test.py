import copclib as copc
import pytest


def test_writer_config():
    # Given a valid file path
    file_path = "../build/test/data/writer_test.copc.laz"

    # Default config
    cfg = copc.LasConfig(0)
    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.GetLasHeader()
    # TODO[Leo]: Update after making our own lazperf headers
    # assert las_header.scale.z == 0.01
    # assert las_header.offset.z == 0
    assert las_header.point_format_id == 0

    writer.Close()

    # Custom config

    cfg = copc.LasConfig(8, copc.vector3(2, 3, 4), copc.vector3(-0.02, -0.03, -40.8))
    cfg.file_source_id = 200
    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.GetLasHeader()
    assert las_header.file_source_id == 200
    assert las_header.point_format_id == 8
    # TODO[Leo]: Update after making our own lazperf headers
    # assert las_header.scale.x == 2
    # assert las_header.offset.x == -0.02
    # assert las_header.scale.y == 3
    # assert las_header.offset.y == -0.03
    # assert las_header.scale.z == 4
    # assert las_header.offset.z == -40.8

    writer.Close()

    # COPC Span

    cfg = copc.LasConfig(0)
    writer = copc.FileWriter(file_path, cfg, 256)

    # todo: use Reader to check all of these
    assert writer.GetCopcHeader().span == 256

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetCopcHeader().span == 256

    # WKT
    cfg = copc.LasConfig(0)
    writer = copc.FileWriter(file_path, cfg, 256, "TEST_WKT")

    # todo: use Reader to check all of these
    assert writer.GetWkt() == "TEST_WKT"

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetWkt() == "TEST_WKT"

    # Copy
    orig = copc.FileReader("../build/test/data/autzen-classified.copc.laz")

    cfg = copc.LasConfig(orig.GetLasHeader(), orig.GetExtraByteVlr())
    writer = copc.FileWriter(file_path, cfg)
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id
    assert reader.GetLasHeader().global_encoding == orig.GetLasHeader().global_encoding
    # assert reader.GetLasHeader().creation.day == orig.GetLasHeader().creation.day
    # assert reader.GetLasHeader().creation.year == orig.GetLasHeader().creation.year
    assert reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id
    assert reader.GetLasHeader().point_format_id == orig.GetLasHeader().point_format_id
    assert (
        reader.GetLasHeader().point_record_length
        == orig.GetLasHeader().point_record_length
    )
    assert reader.GetLasHeader().point_count == 0
    # assert reader.GetLasHeader().scale == reader.GetLasHeader().scale
    # assert reader.GetLasHeader().offset == reader.GetLasHeader().offset


def test_writer_pages():
    # Given a valid file path
    file_path = "../build/test/data/writer_test.copc.laz"

    # Root Page
    writer = copc.FileWriter(file_path, copc.LasConfig(0))

    assert not writer.FindNode(copc.VoxelKey.BaseKey()).IsValid()
    assert not writer.FindNode(copc.VoxelKey.InvalidKey()).IsValid()
    assert not writer.FindNode(copc.VoxelKey(5, 4, 3, 2)).IsValid()

    writer.GetRootPage()
    root_page = writer.GetRootPage()
    assert root_page.IsValid()
    assert root_page.IsPage()
    assert root_page.loaded is True
    with pytest.raises(RuntimeError):
        writer.AddSubPage(root_page, copc.VoxelKey.InvalidKey())

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetCopcHeader().root_hier_offset > 0
    assert reader.GetCopcHeader().root_hier_size == 0
    assert not reader.FindNode(copc.VoxelKey.InvalidKey()).IsValid()

    # Nested page
    writer = copc.FileWriter(file_path, copc.LasConfig(0))

    root_page = writer.GetRootPage()

    sub_page = writer.AddSubPage(root_page, copc.VoxelKey(1, 1, 1, 1))
    assert sub_page.IsPage()
    assert sub_page.IsValid()
    assert sub_page.loaded is True

    with pytest.raises(RuntimeError):
        writer.AddSubPage(sub_page, copc.VoxelKey(1, 1, 1, 0))
        writer.AddSubPage(sub_page, copc.VoxelKey(2, 4, 5, 0))

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetCopcHeader().root_hier_offset > 0
    assert reader.GetCopcHeader().root_hier_size == 32
    assert not reader.FindNode(copc.VoxelKey.InvalidKey()).IsValid()


def test_writer_extra_bytes():
    # Given a valid file path
    file_path = "../build/test/data/writer_test.copc.laz"

    # Data Type 0
    config = copc.LasConfig(7)
    eb_vlr = copc.EbVlr(1)  # Always initialize with the ebCount constructor
    # don't make ebfields yourself unless you set their names correctly
    eb_vlr.items[0].data_type = 0
    eb_vlr.items[0].options = 4
    config.extra_bytes = eb_vlr
    writer = copc.FileWriter(file_path, config)

    assert writer.GetLasHeader().point_record_length == 40  # 36 + 4

    writer.Close()

    reader = copc.FileReader(file_path)
    assert len(reader.GetExtraByteVlr().items) == 1
    assert reader.GetExtraByteVlr().items[0].data_type == 0
    assert reader.GetExtraByteVlr().items[0].options == 4
    assert reader.GetExtraByteVlr().items[0].name == "FIELD_0"
    # TODO
    # assert reader.GetExtraByteVlr().items[0].maxval[2] == 0
    # assert reader.GetExtraByteVlr().items[0].minval[2] == 0
    # assert reader.GetExtraByteVlr().items[0].offset[2] == 0
    # assert reader.GetExtraByteVlr().items[0].scale[2] == 0
    assert reader.GetLasHeader().point_record_length == 40

    # Data type 29

    config = copc.LasConfig(7)
    eb_vlr = copc.EbVlr(1)
    eb_vlr.items[0].data_type = 29
    config.extra_bytes = eb_vlr
    writer = copc.FileWriter(file_path, config)

    assert writer.GetLasHeader().point_record_length == 48  # 36 + 1

    writer.Close()

    reader = copc.FileReader(file_path)
    assert len(reader.GetExtraByteVlr().items) == 1
    assert reader.GetLasHeader().point_record_length == 48

    # Copy Vlr
    reader = copc.FileReader("../build/test/data/autzen-classified.copc.laz")

    in_eb_vlr = reader.GetExtraByteVlr()

    config = copc.LasConfig(3)
    config.extra_bytes = in_eb_vlr
    writer = copc.FileWriter(file_path, config)

    assert writer.GetLasHeader().point_record_length == 36  # 34 + 1 + 1

    writer.Close()

    reader2 = copc.FileReader(file_path)
    assert len(reader2.GetExtraByteVlr().items) == 2
    assert (
        reader2.GetLasHeader().point_record_length
        == reader.GetLasHeader().point_record_length
    )
    assert reader2.GetExtraByteVlr().items == reader.GetExtraByteVlr().items


def test_writer_copy():
    # Given a valid file path
    file_path = "../build/test/data/writer_test.copc.laz"

    reader = copc.FileReader("../build/test/data/autzen-classified.copc.laz")

    cfg = copc.LasConfig(reader.GetLasHeader(), reader.GetExtraByteVlr())
    writer = copc.FileWriter(file_path, cfg)

    root_page = writer.GetRootPage()

    for node in reader.GetAllChildren():
        # only write/compare compressed data or otherwise tests take too long
        writer.AddNodeCompressed(
            root_page, node.key, reader.GetPointDataCompressed(node), node.point_count
        )

    writer.Close()

    new_reader = copc.FileReader(file_path)

    for node in reader.GetAllChildren():
        assert node.IsValid()
        new_node = new_reader.FindNode(node.key)
        assert new_node.IsValid()
        assert new_node.key == node.key
        assert new_node.point_count == node.point_count
        assert new_node.byte_size == node.byte_size
        # assert new_reader.GetPointData(new_node) == reader.GetPointData(node)
        assert new_reader.GetPointDataCompressed(
            new_node
        ) == reader.GetPointDataCompressed(node)

    # we can do one uncompressed comparison here
    assert new_reader.GetPointData(
        new_reader.FindNode(copc.VoxelKey(5, 9, 7, 0))
    ) == reader.GetPointData(reader.FindNode(copc.VoxelKey(5, 9, 7, 0)))
