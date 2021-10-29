import copclib as copc
import pytest
from sys import float_info


def test_writer_config():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    # Default config
    cfg = copc.CopcConfigWriter(6)
    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.copc_config.las_header
    assert las_header.scale == [0.01, 0.01, 0.01]
    assert las_header.offset == [0.0, 0.0, 0.0]
    assert las_header.point_format_id == 6

    str(cfg)

    with pytest.raises(RuntimeError):
        assert copc.CopcConfigWriter(5)
        assert copc.CopcConfigWriter(9)

    writer.Close()

    # Custom config

    cfg = copc.CopcConfigWriter(8, [2, 3, 4], [-0.02, -0.03, -40.8])
    cfg.las_header.file_source_id = 200

    # Test LasHeaderConfig attributes
    cfg.las_header.creation_day = 18
    assert cfg.las_header.creation_day == 18
    cfg.las_header.creation_year = 11
    assert cfg.las_header.creation_year == 11

    # Test checks on string attributes
    cfg.las_header.guid = "test_string"
    assert cfg.las_header.guid == "test_string"
    with pytest.raises(RuntimeError):
        cfg.las_header.guid = "a" * 17

    cfg.las_header.system_identifier = "test_string"
    assert cfg.las_header.system_identifier == "test_string"
    with pytest.raises(RuntimeError):
        cfg.las_header.system_identifier = "a" * 33

    cfg.las_header.generating_software = "test_string"
    assert cfg.las_header.generating_software == "test_string"
    with pytest.raises(RuntimeError):
        cfg.las_header.generating_software = "a" * 33

    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.copc_config.las_header
    assert las_header.file_source_id == 200
    assert las_header.point_format_id == 8
    assert las_header.scale == [2.0, 3.0, 4.0]
    assert las_header.offset == [-0.02, -0.03, -40.8]

    writer.Close()

    # COPC Spacing

    cfg = copc.CopcConfigWriter(6)
    cfg.copc_info.spacing = 10
    writer = copc.FileWriter(file_path, cfg)

    assert writer.copc_config.copc_info.spacing == 10

    writer.copc_config.copc_info.spacing = 15

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.copc_info.spacing == 15

    # Extents
    cfg = copc.CopcConfigWriter(6)
    writer = copc.FileWriter(file_path, cfg)

    extents = writer.copc_config.copc_extents

    extents.intensity.minimum = -1.0
    extents.intensity.maximum = 1

    extents.classification.minimum = -float_info.max
    extents.classification.maximum = float_info.max

    assert (
        writer.copc_config.copc_extents.intensity.minimum == extents.intensity.minimum
    )
    assert (
        writer.copc_config.copc_extents.intensity.maximum == extents.intensity.maximum
    )
    assert (
        writer.copc_config.copc_extents.classification.minimum
        == extents.classification.minimum
    )
    assert (
        writer.copc_config.copc_extents.classification.maximum
        == extents.classification.maximum
    )

    writer.Close()

    reader = copc.FileReader(file_path)
    assert (
        reader.copc_config.copc_extents.intensity.minimum == extents.intensity.minimum
    )
    assert (
        reader.copc_config.copc_extents.intensity.maximum == extents.intensity.maximum
    )
    assert (
        reader.copc_config.copc_extents.classification.minimum
        == extents.classification.minimum
    )
    assert (
        reader.copc_config.copc_extents.classification.maximum
        == extents.classification.maximum
    )

    # WKT
    cfg = copc.CopcConfigWriter(6, wkt="TEST_WKT")
    writer = copc.FileWriter(file_path, cfg)

    assert writer.copc_config.wkt == "TEST_WKT"

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.wkt == "TEST_WKT"

    # Copy
    orig = copc.FileReader("autzen-classified.copc.laz")

    cfg = orig.copc_config
    writer = copc.FileWriter(file_path, cfg)

    writer.copc_config.las_header.min = (1, 1, 1)
    writer.copc_config.las_header.max = (50, 50, 50)

    assert writer.copc_config.las_header.min == (1, 1, 1)
    assert writer.copc_config.las_header.max == (50, 50, 50)

    writer.Close()

    reader = copc.FileReader(file_path)
    assert (
        reader.copc_config.las_header.file_source_id
        == orig.copc_config.las_header.file_source_id
    )
    assert (
        reader.copc_config.las_header.global_encoding
        == orig.copc_config.las_header.global_encoding
    )
    assert (
        reader.copc_config.las_header.creation_day
        == orig.copc_config.las_header.creation_day
    )
    assert (
        reader.copc_config.las_header.creation_year
        == orig.copc_config.las_header.creation_year
    )
    assert (
        reader.copc_config.las_header.file_source_id
        == orig.copc_config.las_header.file_source_id
    )
    assert (
        reader.copc_config.las_header.point_format_id
        == orig.copc_config.las_header.point_format_id
    )
    assert (
        reader.copc_config.las_header.point_record_length
        == orig.copc_config.las_header.point_record_length
    )
    assert reader.copc_config.las_header.point_count == 0
    assert reader.copc_config.las_header.min == (1, 1, 1)
    assert reader.copc_config.las_header.max == (50, 50, 50)

    # Update
    min1 = (-800, 300, 800)
    max1 = (5000, 8444, 3333)
    min2 = (-20, -30, -40)
    max2 = (20, 30, 40)
    points_by_return = list(range(15))

    cfg = copc.CopcConfigWriter(6, wkt="TEST_WKT")
    cfg.las_header.min = min1
    cfg.las_header.max = max1
    cfg.copc_info.spacing = 10
    writer = copc.FileWriter(file_path, cfg)

    assert writer.copc_config.las_header.min == min1
    assert writer.copc_config.las_header.max == max1
    assert writer.copc_config.las_header.points_by_return == [0] * 15

    with pytest.raises(TypeError):
        writer.copc_config.las_header.points_by_return = [20] * 800

    writer.copc_config.las_header.min = min2
    writer.copc_config.las_header.max = max2
    writer.copc_config.las_header.points_by_return = points_by_return

    assert writer.copc_config.las_header.min == min2
    assert writer.copc_config.las_header.max == max2
    assert writer.copc_config.las_header.points_by_return == points_by_return

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.min == min2
    assert reader.copc_config.las_header.max == max2
    assert reader.copc_config.las_header.points_by_return == points_by_return


def test_writer_pages():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    writer = copc.FileWriter(file_path, copc.CopcConfigWriter(6))

    assert not writer.FindNode(copc.VoxelKey.RootKey()).IsValid()
    assert not writer.FindNode(copc.VoxelKey.InvalidKey()).IsValid()
    assert not writer.FindNode((5, 4, 3, 2)).IsValid()

    # Root Page
    writer.GetRootPage()
    root_page = writer.GetRootPage()
    assert root_page.IsValid()
    assert root_page.IsPage()
    assert root_page.loaded is True
    with pytest.raises(RuntimeError):
        writer.AddSubPage(parent_page=root_page, key=copc.VoxelKey.InvalidKey())

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.copc_info.root_hier_offset > 0
    assert reader.copc_config.copc_info.root_hier_size == 0
    assert not reader.FindNode(key=copc.VoxelKey.InvalidKey()).IsValid()

    # Nested page
    writer = copc.FileWriter(file_path, copc.CopcConfigWriter(6))

    root_page = writer.GetRootPage()

    sub_page = writer.AddSubPage(root_page, (1, 1, 1, 1))
    assert sub_page.IsPage()
    assert sub_page.IsValid()
    assert sub_page.loaded is True

    with pytest.raises(RuntimeError):
        writer.AddSubPage(sub_page, (1, 1, 1, 0))
        writer.AddSubPage(sub_page, (2, 4, 5, 0))

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.copc_info.root_hier_offset > 0
    assert reader.copc_config.copc_info.root_hier_size == 32
    assert not reader.FindNode(copc.VoxelKey.InvalidKey()).IsValid()


def test_writer_copy():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    reader = copc.FileReader("autzen-classified.copc.laz")

    cfg = reader.copc_config
    writer = copc.FileWriter(file_path, cfg)

    root_page = writer.GetRootPage()

    for node in reader.GetAllNodes():
        # only write/compare compressed data or otherwise tests take too long
        writer.AddNodeCompressed(
            root_page, node.key, reader.GetPointDataCompressed(node), node.point_count
        )

    writer.Close()

    # validate
    new_reader = copc.FileReader(file_path)
    for node in reader.GetAllNodes():
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
        new_reader.FindNode((5, 9, 7, 0))
    ) == reader.GetPointData(reader.FindNode((5, 9, 7, 0)))


def test_writer_copy_and_update():

    file_path = "writer_test.copc.laz"

    orig = copc.FileReader("autzen-classified.copc.laz")

    cfg = orig.copc_config

    new_scale = copc.Vector3(10, 10, 10)
    new_offset = (100, 100, 100)
    new_wkt = "test_wkt"
    new_has_extended_stats = True
    new_eb_vlr = copc.EbVlr(2)

    # Update Scale
    writer = copc.FileWriter(file_path, cfg, new_scale=new_scale)

    assert writer.copc_config.las_header.scale == new_scale
    assert writer.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert writer.copc_config.Wkt() == orig.copc_config.Wkt()
    assert (
        writer.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert (
        writer.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.scale == new_scale
    assert reader.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert reader.copc_config.Wkt() == orig.copc_config.Wkt()
    assert (
        reader.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert (
        reader.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )

    # Update Offset
    writer = copc.FileWriter(file_path, cfg, new_offset=new_offset)

    assert writer.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert writer.copc_config.las_header.offset == new_offset
    assert writer.copc_config.Wkt() == orig.copc_config.Wkt()
    assert (
        writer.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert (
        writer.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert reader.copc_config.las_header.offset == new_offset
    assert reader.copc_config.Wkt() == orig.copc_config.Wkt()
    assert (
        reader.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert (
        reader.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )

    # Update WKT
    writer = copc.FileWriter(file_path, cfg, new_wkt=new_wkt)

    assert writer.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert writer.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert writer.copc_config.Wkt() == new_wkt
    assert (
        writer.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert (
        writer.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert reader.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert reader.copc_config.Wkt() == new_wkt
    assert (
        reader.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert (
        reader.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )

    # Update Extra Byte VLR

    writer = copc.FileWriter(file_path, cfg, new_eb_vlr=new_eb_vlr)

    assert writer.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert writer.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert writer.copc_config.Wkt() == orig.copc_config.Wkt()
    assert writer.copc_config.ExtraBytesVlr().items.size() == new_eb_vlr.items.size()
    assert (
        writer.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert reader.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert reader.copc_config.Wkt() == orig.copc_config.Wkt()
    assert reader.copc_config.ExtraBytesVlr().items.size() == new_eb_vlr.items.size()
    assert (
        reader.copc_config.CopcExtents().HasExtendedStats()
        == orig.copc_config.CopcExtents().HasExtendedStats()
    )

    # Update HasExtendedStats

    writer = copc.FileWriter(
        file_path, cfg, new_has_extended_stats=new_has_extended_stats
    )

    assert writer.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert writer.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert writer.copc_config.Wkt() == orig.copc_config.Wkt()
    assert (
        writer.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert writer.copc_config.CopcExtents().HasExtendedStats() == new_has_extended_stats
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.scale == orig.copc_config.las_header.scale
    assert reader.copc_config.las_header.offset == orig.copc_config.las_header.offset
    assert reader.copc_config.Wkt() == orig.copc_config.Wkt()
    assert (
        reader.copc_config.ExtraBytesVlr().items.size()
        == orig.copc_config.ExtraBytesVlr().items.size()
    )
    assert reader.copc_config.CopcExtents().HasExtendedStats() == new_has_extended_stats

    # Update All

    writer = copc.FileWriter(
        file_path,
        cfg,
        new_scale,
        new_offset,
        new_wkt,
        new_eb_vlr,
        new_has_extended_stats,
    )

    assert writer.copc_config.las_header.scale == new_scale
    assert writer.copc_config.las_header.offset == new_offset
    assert writer.copc_config.Wkt() == new_wkt
    assert writer.copc_config.ExtraBytesVlr().items.size() == new_eb_vlr.items.size()
    assert writer.copc_config.CopcExtents().HasExtendedStats() == new_has_extended_stats
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.las_header.scale == new_scale
    assert reader.copc_config.las_header.offset == new_offset
    assert reader.copc_config.Wkt() == new_wkt
    assert reader.copc_config.ExtraBytesVlr().items.size() == new_eb_vlr.items.size()
    assert reader.copc_config.CopcExtents().HasExtendedStats() == new_has_extended_stats


def test_check_spatial_bounds():

    file_path = "writer_test.copc.laz"

    cfg = copc.CopcConfigWriter(7, (0.1, 0.1, 0.1), (50, 50, 50))
    cfg.las_header.min = (-10, -10, -5)
    cfg.las_header.max = (10, 10, 5)
    verbose = False

    writer = copc.FileWriter(file_path, cfg)
    header = writer.copc_config.las_header

    ## Checks on las header bounds

    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    # point has getters/setters for all attributes
    point.x = 10
    point.y = 10
    point.z = 5

    points.AddPoint(point)

    writer.AddNode(writer.GetRootPage(), (1, 1, 1, 1), points)
    writer.Close()

    reader = copc.FileReader(file_path)

    assert reader.ValidateSpatialBounds(verbose) == True

    # Las Header Bounds check (node outside)
    writer = copc.FileWriter(file_path, cfg)

    header = writer.copc_config.las_header

    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    point.x = 10
    point.y = 10
    point.z = 5.1

    points.AddPoint(point)
    writer.AddNode(writer.GetRootPage(), (2, 3, 3, 3), points)
    writer.Close()

    reader = copc.FileReader(file_path)

    assert reader.ValidateSpatialBounds(verbose) == False

    # Las Header Bounds check (node intersects)
    writer = copc.FileWriter(file_path, cfg)

    header = writer.copc_config.las_header

    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    point.x = 10
    point.y = 10
    point.z = 5.1

    points.AddPoint(point)
    writer.AddNode(writer.GetRootPage(), (1, 1, 1, 1), points)
    writer.Close()

    reader = copc.FileReader(file_path)

    assert reader.ValidateSpatialBounds(verbose) == False

    # Node Bounds check
    writer = copc.FileWriter(file_path, cfg)

    header = writer.copc_config.las_header
    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    point.x = 0.1
    point.y = 0.1
    point.z = 0.1

    points.AddPoint(point)
    writer.AddNode(writer.GetRootPage(), (1, 0, 0, 0), points)
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.ValidateSpatialBounds(verbose) == False
