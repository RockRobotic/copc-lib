import copclib as copc
import pytest


def test_writer_config():
    # Given a valid file path
    file_path = "data/writer_test.copc.laz"

    # Default config
    cfg = copc.LasHeaderConfig(0)
    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.GetLasHeader()
    assert las_header.scale == [0.01, 0.01, 0.01]
    assert las_header.offset == [0.0, 0.0, 0.0]
    assert las_header.point_format_id == 0

    str(cfg)

    writer.Close()

    # Custom config

    cfg = copc.LasHeaderConfig(8, [2, 3, 4], [-0.02, -0.03, -40.8])
    cfg.file_source_id = 200

    # Test LasHeaderConfig attributes
    cfg.creation_day = 18
    assert cfg.creation_day == 18
    cfg.creation_year = 11
    assert cfg.creation_year == 11

    # Test checks on string attributes
    cfg.guid = "test_string"
    assert cfg.guid == "test_string"
    with pytest.raises(RuntimeError):
        cfg.guid = "a" * 17

    cfg.system_identifier = "test_string"
    assert cfg.system_identifier == "test_string"
    with pytest.raises(RuntimeError):
        cfg.system_identifier = "a" * 33

    cfg.generating_software = "test_string"
    assert cfg.generating_software == "test_string"
    with pytest.raises(RuntimeError):
        cfg.generating_software = "a" * 33

    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.GetLasHeader()
    assert las_header.file_source_id == 200
    assert las_header.point_format_id == 8
    assert las_header.scale == [2.0, 3.0, 4.0]
    assert las_header.offset == [-0.02, -0.03, -40.8]

    writer.Close()

    # COPC Span

    cfg = copc.LasHeaderConfig(0)
    writer = copc.FileWriter(file_path, cfg, 256)

    # todo: use Reader to check all of these
    assert writer.GetCopcHeader().span == 256

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetCopcHeader().span == 256

    # WKT
    cfg = copc.LasHeaderConfig(0)
    writer = copc.FileWriter(file_path, cfg, 256, "TEST_WKT")

    # todo: use Reader to check all of these
    assert writer.GetWkt() == "TEST_WKT"

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetWkt() == "TEST_WKT"

    # Copy
    orig = copc.FileReader("data/autzen-classified.copc.laz")

    cfg = copc.LasHeaderConfig(orig.GetLasHeader(), orig.GetExtraByteVlr())
    writer = copc.FileWriter(file_path, cfg)
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id
    assert reader.GetLasHeader().global_encoding == orig.GetLasHeader().global_encoding
    assert reader.GetLasHeader().creation_day == orig.GetLasHeader().creation_day
    assert reader.GetLasHeader().creation_year == orig.GetLasHeader().creation_year
    assert reader.GetLasHeader().file_source_id == orig.GetLasHeader().file_source_id
    assert reader.GetLasHeader().point_format_id == orig.GetLasHeader().point_format_id
    assert (
        reader.GetLasHeader().point_record_length
        == orig.GetLasHeader().point_record_length
    )
    assert reader.GetLasHeader().point_count == 0
    assert reader.GetLasHeader().scale == reader.GetLasHeader().scale
    assert reader.GetLasHeader().offset == reader.GetLasHeader().offset

    # Update
    min1 = copc.Vector3([-800, 300, 800])
    max1 = copc.Vector3([5000, 8444, 3333])
    min2 = copc.Vector3([-20, -30, -40])
    max2 = copc.Vector3([20, 30, 40])
    points_by_return = list(range(15))

    cfg = copc.LasHeaderConfig(0)
    cfg.min = min1
    cfg.max = max1
    writer = copc.FileWriter(file_path, cfg, 256, "TEST_WKT")

    # todo: use Reader to check all of these
    assert writer.GetLasHeader().min == min1
    assert writer.GetLasHeader().max == max1
    assert writer.GetLasHeader().points_by_return_14 == [0] * 15

    with pytest.raises(TypeError):
        writer.SetPointsByReturn([20] * 800)

    writer.SetMin(min2)
    writer.SetMax(max2)
    writer.SetPointsByReturn(points_by_return)

    assert writer.GetLasHeader().min == min2
    assert writer.GetLasHeader().max == max2
    assert writer.GetLasHeader().points_by_return_14 == points_by_return

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetLasHeader().min == min2
    assert reader.GetLasHeader().max == max2
    assert reader.GetLasHeader().points_by_return_14 == points_by_return


def test_writer_pages():
    # Given a valid file path
    file_path = "data/writer_test.copc.laz"

    # Root Page
    writer = copc.FileWriter(file_path, copc.LasHeaderConfig(0))

    assert not writer.FindNode(copc.VoxelKey().BaseKey()).IsValid()
    assert not writer.FindNode(copc.VoxelKey().InvalidKey()).IsValid()
    assert not writer.FindNode(copc.VoxelKey(5, 4, 3, 2)).IsValid()

    writer.GetRootPage()
    root_page = writer.GetRootPage()
    assert root_page.IsValid()
    assert root_page.IsPage()
    assert root_page.loaded is True
    with pytest.raises(RuntimeError):
        writer.AddSubPage(parent_page=root_page, key=copc.VoxelKey().InvalidKey())

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.GetCopcHeader().root_hier_offset > 0
    assert reader.GetCopcHeader().root_hier_size == 0
    assert not reader.FindNode(key=copc.VoxelKey().InvalidKey()).IsValid()

    # Nested page
    writer = copc.FileWriter(file_path, copc.LasHeaderConfig(0))

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
    assert not reader.FindNode(copc.VoxelKey().InvalidKey()).IsValid()


def test_writer_copy():
    # Given a valid file path
    file_path = "data/writer_test.copc.laz"

    reader = copc.FileReader("data/autzen-classified.copc.laz")

    cfg = copc.LasHeaderConfig(reader.GetLasHeader(), reader.GetExtraByteVlr())
    writer = copc.FileWriter(file_path, cfg)

    root_page = writer.GetRootPage()

    for node in reader.GetAllChildren():
        # only write/compare compressed data or otherwise tests take too long
        writer.AddNodeCompressed(
            root_page, node.key, reader.GetPointDataCompressed(node), node.point_count
        )

    writer.Close()

    # validate
    new_reader = copc.FileReader(file_path)
    for node in reader.GetAllChildren():
        assert node.IsValid()
        new_node = new_reader.FindNode(node.key)
        assert new_node.IsValid()
        assert new_node.key == node.key
        assert new_node.point_count == node.point_count
        assert new_node.byte_size == node.byte_size
        assert new_reader.GetPointDataCompressed(
            new_node
        ) == reader.GetPointDataCompressed(node)

    # we can do one uncompressed comparison here
    assert new_reader.GetPointData(
        new_reader.FindNode(copc.VoxelKey(5, 9, 7, 0))
    ) == reader.GetPointData(reader.FindNode(copc.VoxelKey(5, 9, 7, 0)))
