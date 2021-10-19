import copclib as copc
import pytest
from sys import float_info


def test_writer_config():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    # Default config
    cfg = copc.CopcConfig(6)
    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.las_header
    assert las_header.scale == [0.01, 0.01, 0.01]
    assert las_header.offset == [0.0, 0.0, 0.0]
    assert las_header.point_format_id == 6

    str(cfg)

    with pytest.raises(RuntimeError):
        assert copc.CopcConfig(5)
        assert copc.CopcConfig(9)

    writer.Close()

    # Custom config

    cfg = copc.CopcConfig(8, [2, 3, 4], [-0.02, -0.03, -40.8])
    cfg.las_header_base.file_source_id = 200

    # Test LasHeaderConfig attributes
    cfg.las_header_base.creation_day = 18
    assert cfg.las_header_base.creation_day == 18
    cfg.las_header_base.creation_year = 11
    assert cfg.las_header_base.creation_year == 11

    # Test checks on string attributes
    cfg.las_header_base.guid = "test_string"
    assert cfg.las_header_base.guid == "test_string"
    with pytest.raises(RuntimeError):
        cfg.las_header_base.guid = "a" * 17

    cfg.las_header_base.system_identifier = "test_string"
    assert cfg.las_header_base.system_identifier == "test_string"
    with pytest.raises(RuntimeError):
        cfg.las_header_base.system_identifier = "a" * 33

    cfg.las_header_base.generating_software = "test_string"
    assert cfg.las_header_base.generating_software == "test_string"
    with pytest.raises(RuntimeError):
        cfg.las_header_base.generating_software = "a" * 33

    writer = copc.FileWriter(file_path, cfg)

    las_header = writer.las_header
    assert las_header.file_source_id == 200
    assert las_header.point_format_id == 8
    assert las_header.scale == [2.0, 3.0, 4.0]
    assert las_header.offset == [-0.02, -0.03, -40.8]

    writer.Close()

    # COPC Spacing

    cfg = copc.CopcConfig(6)
    cfg.copc_info.spacing = 10
    writer = copc.FileWriter(file_path, cfg)

    assert writer.copc_info.spacing == 10

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_info.spacing == 10

    # Extents
    cfg = copc.CopcConfig(6)
    writer = copc.FileWriter(file_path, cfg)

    extents = writer.copc_extents

    extents.intensity.minimum = -1.0
    extents.intensity.maximum = 1

    extents.classification.minimum = -float_info.max
    extents.classification.maximum = float_info.max

    writer.copc_extents = extents

    assert writer.copc_extents.intensity.minimum == extents.intensity.minimum
    assert writer.copc_extents.intensity.maximum == extents.intensity.maximum
    assert writer.copc_extents.classification.minimum == extents.classification.minimum
    assert writer.copc_extents.classification.maximum == extents.classification.maximum

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_extents.intensity.minimum == extents.intensity.minimum
    assert reader.copc_extents.intensity.maximum == extents.intensity.maximum
    assert reader.copc_extents.classification.minimum == extents.classification.minimum
    assert reader.copc_extents.classification.maximum == extents.classification.maximum

    # WKT
    cfg = copc.CopcConfig(6)
    cfg.wkt = "TEST_WKT"
    writer = copc.FileWriter(file_path, cfg)

    assert writer.wkt == "TEST_WKT"

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.wkt == "TEST_WKT"

    # Copy
    orig = copc.FileReader("autzen-classified.copc.laz")

    cfg = orig.GetCopcConfig()
    writer = copc.FileWriter(file_path, cfg)
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.las_header.file_source_id == orig.las_header.file_source_id
    assert reader.las_header.global_encoding == orig.las_header.global_encoding
    assert reader.las_header.creation_day == orig.las_header.creation_day
    assert reader.las_header.creation_year == orig.las_header.creation_year
    assert reader.las_header.file_source_id == orig.las_header.file_source_id
    assert reader.las_header.point_format_id == orig.las_header.point_format_id
    assert reader.las_header.point_record_length == orig.las_header.point_record_length
    assert reader.las_header.point_count == 0
    assert reader.las_header.scale == reader.las_header.scale
    assert reader.las_header.offset == reader.las_header.offset

    # Update
    min1 = (-800, 300, 800)
    max1 = (5000, 8444, 3333)
    min2 = (-20, -30, -40)
    max2 = (20, 30, 40)
    points_by_return = list(range(15))

    cfg = copc.CopcConfig(6)
    cfg.las_header_base.min = min1
    cfg.las_header_base.max = max1
    cfg.copc_info.spacing = 10
    cfg.wkt = "TEST_WKT"
    writer = copc.FileWriter(file_path, cfg)

    assert writer.las_header.min == min1
    assert writer.las_header.max == max1
    assert writer.las_header.points_by_return == [0] * 15

    with pytest.raises(TypeError):
        writer.SetPointsByReturn([20] * 800)

    writer.SetMin(min2)
    writer.SetMax(max2)
    writer.SetPointsByReturn(points_by_return)

    assert writer.las_header.min == min2
    assert writer.las_header.max == max2
    assert writer.las_header.points_by_return == points_by_return

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.las_header.min == min2
    assert reader.las_header.max == max2
    assert reader.las_header.points_by_return == points_by_return


def test_writer_pages():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    writer = copc.FileWriter(file_path, copc.CopcConfig(6))

    assert not writer.FindNode(copc.VoxelKey.BaseKey()).IsValid()
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
    assert reader.copc_info.root_hier_offset > 0
    assert reader.copc_info.root_hier_size == 0
    assert not reader.FindNode(key=copc.VoxelKey.InvalidKey()).IsValid()

    # Nested page
    writer = copc.FileWriter(file_path, copc.CopcConfig(6))

    root_page = writer.GetRootPage()

    sub_page = writer.AddSubPage(root_page, copc.VoxelKey(1, 1, 1, 1))
    assert sub_page.IsPage()
    assert sub_page.IsValid()
    assert sub_page.loaded is True

    with pytest.raises(RuntimeError):
        writer.AddSubPage(sub_page, (1, 1, 1, 0))
        writer.AddSubPage(sub_page, (2, 4, 5, 0))

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_info.root_hier_offset > 0
    assert reader.copc_info.root_hier_size == 32
    assert not reader.FindNode(copc.VoxelKey.InvalidKey()).IsValid()


def test_writer_copy():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    reader = copc.FileReader("autzen-classified.copc.laz")

    cfg = reader.GetCopcConfig()
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
        new_reader.FindNode((5, 9, 7, 0))
    ) == reader.GetPointData(reader.FindNode((5, 9, 7, 0)))


def test_check_spatial_bounds():

    file_path = "writer_test.copc.laz"

    cfg = copc.CopcConfig(7, (0.1, 0.1, 0.1), (50, 50, 50))
    cfg.las_header_base.min = (-10, -10, -5)
    cfg.las_header_base.max = (10, 10, 5)
    verbose = False

    writer = copc.FileWriter(file_path, cfg)
    header = writer.las_header

    ## Checks on las header bounds

    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    # point has getters/setters for all attributes
    point.X = 10
    point.Y = 10
    point.Z = 5

    points.AddPoint(point)

    writer.AddNode(writer.GetRootPage(), copc.VoxelKey(1, 1, 1, 1), points)
    writer.Close()

    reader = copc.FileReader(file_path)

    assert reader.ValidateSpatialBounds(verbose) == True

    # Las Header Bounds check (node outside)
    writer = copc.FileWriter(file_path, cfg)

    header = writer.las_header

    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    point.X = 10
    point.Y = 10
    point.Z = 5.1

    points.AddPoint(point)
    writer.AddNode(writer.GetRootPage(), copc.VoxelKey(2, 3, 3, 3), points)
    writer.Close()

    reader = copc.FileReader(file_path)

    assert reader.ValidateSpatialBounds(verbose) == False

    # Las Header Bounds check (node intersects)
    writer = copc.FileWriter(file_path, cfg)

    header = writer.las_header

    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    point.X = 10
    point.Y = 10
    point.Z = 5.1

    points.AddPoint(point)
    writer.AddNode(writer.GetRootPage(), copc.VoxelKey(1, 1, 1, 1), points)
    writer.Close()

    reader = copc.FileReader(file_path)

    assert reader.ValidateSpatialBounds(verbose) == False

    # Node Bounds check
    writer = copc.FileWriter(file_path, cfg)

    header = writer.las_header
    points = copc.Points(header.point_format_id, header.scale, header.offset)

    point = points.CreatePoint()
    point.X = 0.1
    point.Y = 0.1
    point.Z = 0.1

    points.AddPoint(point)
    writer.AddNode(writer.GetRootPage(), copc.VoxelKey(1, 0, 0, 0), points)
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.ValidateSpatialBounds(verbose) == False
