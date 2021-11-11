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
    file_path = "writer_test.copc.laz"

    writer = copc.FileWriter(file_path, copc.CopcConfigWriter(6))

    assert not writer.FindNode(copc.VoxelKey.RootKey()).IsValid()
    assert not writer.FindNode(copc.VoxelKey.InvalidKey()).IsValid()
    assert not writer.FindNode((5, 4, 3, 2)).IsValid()

    # Root Page

    header = writer.copc_config.las_header
    points = copc.Points(header.point_format_id, header.scale, header.offset)
    points.AddPoint(points.CreatePoint())
    # Add a node with root key as page
    writer.AddNode((1, 1, 1, 1), points, copc.VoxelKey.RootKey())

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.copc_info.root_hier_offset > 0
    assert reader.copc_config.copc_info.root_hier_size == 32
    assert not reader.FindNode(key=copc.VoxelKey.InvalidKey()).IsValid()

    # Nested page
    writer = copc.FileWriter(file_path, copc.CopcConfigWriter(6))

    header = writer.copc_config.las_header
    points = copc.Points(header.point_format_id, header.scale, header.offset)
    points.AddPoint(points.CreatePoint())

    writer.AddNode((1, 1, 1, 1), points, page_key=(1, 1, 1, 1))
    writer.AddNode((3, 4, 4, 4), points, page_key=(2, 2, 2, 2))
    writer.AddNode((2, 0, 2, 2), points, page_key=(1, 0, 1, 1))

    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.copc_config.copc_info.root_hier_offset > 0
    assert (
        reader.copc_config.copc_info.root_hier_size == 64
    )  # size of two sub pages of the root page
    assert not reader.FindNode(copc.VoxelKey.InvalidKey()).IsValid()

    page_keys = reader.GetPageList()
    assert len(page_keys) == 4
    assert (1, 1, 1, 1) in page_keys
    assert (2, 2, 2, 2) in page_keys
    assert (1, 0, 1, 1) in page_keys

    # Change Node Page
    writer = copc.FileWriter(file_path, copc.CopcConfigWriter(6))

    header = writer.copc_config.las_header
    points = copc.Points(header.point_format_id, header.scale, header.offset)
    points.AddPoint(points.CreatePoint())

    writer.AddNode((3, 4, 4, 4), points, page_key=(2, 2, 2, 2))

    writer.ChangeNodePage((3, 4, 4, 4), (1, 1, 1, 1))

    with pytest.raises(RuntimeError):
        # Check for validity
        writer.ChangeNodePage((3, 4, 4, 3), (1, 0, 0, 0))  # Node doesn't exist
        writer.ChangeNodePage((3, 4, 4, -1), (1, 0, 0, 0))  # Invalid Node
        writer.ChangeNodePage((3, 4, 4, 4), (1, 0, 0, -1))  # Invalid New Page
        # Check for parental link
        writer.ChangeNodePage((3, 4, 4, 4), (1, 0, 0, 0))
    writer.Close()

    reader = copc.FileReader(file_path)
    node = reader.FindNode((3, 4, 4, 4))
    assert node.page_key == (1, 1, 1, 1)


def test_writer_copy():
    # Given a valid file path
    file_path = "writer_test.copc.laz"

    reader = copc.FileReader("autzen-classified.copc.laz")

    cfg = reader.copc_config
    writer = copc.FileWriter(file_path, cfg)

    for node in reader.GetAllNodes():
        # only write/compare compressed data or otherwise tests take too long
        writer.AddNodeCompressed(
            node.key, reader.GetPointDataCompressed(node), node.point_count
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

    writer.AddNode((1, 1, 1, 1), points)
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
    writer.AddNode((2, 3, 3, 3), points)
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
    writer.AddNode((1, 1, 1, 1), points)
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
    writer.AddNode((1, 0, 0, 0), points)
    writer.Close()

    reader = copc.FileReader(file_path)
    assert reader.ValidateSpatialBounds(verbose) == False
