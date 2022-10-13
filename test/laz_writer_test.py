import copclib as copc
import pytest
import os

from .utils import get_autzen_file, get_data_dir


def test_writer_config():
    # Given a valid file path
    file_path = os.path.join(get_data_dir(), "writer_test.laz")

    # Default config
    cfg = copc.LazConfigWriter(6)
    writer = copc.LazWriter(file_path, cfg)

    las_header = writer.laz_config.las_header
    assert las_header.scale == [0.01, 0.01, 0.01]
    assert las_header.offset == [0.0, 0.0, 0.0]
    assert las_header.point_format_id == 6

    str(cfg)

    with pytest.raises(RuntimeError):
        assert copc.LazConfigWriter(5)
        assert copc.LazConfigWriter(9)

    writer.Close()

    # Given an invalid file path
    with pytest.raises(RuntimeError):
        assert copc.LazWriter(
            os.path.join(get_data_dir(), "invalid_path", "non_existant_file.copc.laz"),
            cfg,
        )

    # Custom config

    cfg = copc.LazConfigWriter(8, [2, 3, 4], [-0.02, -0.03, -40.8])
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

    writer = copc.LazWriter(file_path, cfg)

    las_header = writer.laz_config.las_header
    assert las_header.file_source_id == 200
    assert las_header.point_format_id == 8
    assert las_header.scale == [2.0, 3.0, 4.0]
    assert las_header.offset == [-0.02, -0.03, -40.8]

    writer.Close()

    # WKT
    cfg = copc.LazConfigWriter(6, wkt="TEST_WKT")
    writer = copc.LazWriter(file_path, cfg)

    assert writer.laz_config.wkt == "TEST_WKT"

    writer.Close()

    reader = copc.LazReader(file_path)
    assert reader.laz_config.wkt == "TEST_WKT"

    # Copy
    orig = copc.LazReader(get_autzen_file())

    cfg = orig.laz_config
    writer = copc.LazWriter(file_path, cfg)

    writer.laz_config.las_header.min = (1, 1, 1)
    writer.laz_config.las_header.max = (50, 50, 50)

    assert writer.laz_config.las_header.min == (1, 1, 1)
    assert writer.laz_config.las_header.max == (50, 50, 50)

    writer.Close()

    reader = copc.LazReader(file_path)
    assert (
        reader.laz_config.las_header.file_source_id
        == orig.laz_config.las_header.file_source_id
    )
    assert (
        reader.laz_config.las_header.global_encoding
        == orig.laz_config.las_header.global_encoding
    )
    assert (
        reader.laz_config.las_header.creation_day
        == orig.laz_config.las_header.creation_day
    )
    assert (
        reader.laz_config.las_header.creation_year
        == orig.laz_config.las_header.creation_year
    )
    assert (
        reader.laz_config.las_header.file_source_id
        == orig.laz_config.las_header.file_source_id
    )
    assert (
        reader.laz_config.las_header.point_format_id
        == orig.laz_config.las_header.point_format_id
    )
    assert (
        reader.laz_config.las_header.point_record_length
        == orig.laz_config.las_header.point_record_length
    )
    assert reader.laz_config.las_header.point_count == 0
    assert reader.laz_config.las_header.min == (1, 1, 1)
    assert reader.laz_config.las_header.max == (50, 50, 50)

    # Update
    min1 = (-800, 300, 800)
    max1 = (5000, 8444, 3333)
    min2 = (-20, -30, -40)
    max2 = (20, 30, 40)
    points_by_return = list(range(15))

    cfg = copc.LazConfigWriter(6, wkt="TEST_WKT")
    cfg.las_header.min = min1
    cfg.las_header.max = max1
    cfg.copc_info.spacing = 10
    writer = copc.LazWriter(file_path, cfg)

    assert writer.laz_config.las_header.min == min1
    assert writer.laz_config.las_header.max == max1
    assert writer.laz_config.las_header.points_by_return == [0] * 15

    with pytest.raises(TypeError):
        writer.laz_config.las_header.points_by_return = [20] * 800

    writer.laz_config.las_header.min = min2
    writer.laz_config.las_header.max = max2
    writer.laz_config.las_header.points_by_return = points_by_return

    assert writer.laz_config.las_header.min == min2
    assert writer.laz_config.las_header.max == max2
    assert writer.laz_config.las_header.points_by_return == points_by_return

    writer.Close()

    reader = copc.LazReader(file_path)
    assert reader.laz_config.las_header.min == min2
    assert reader.laz_config.las_header.max == max2
    assert reader.laz_config.las_header.points_by_return == points_by_return


def test_writer_copy():
    # Given a valid file path
    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")

    reader = copc.FileReader(get_autzen_file())

    cfg = reader.copc_config
    writer = copc.LazWriter(file_path, cfg)

    num_pts = 0
    for node in reader.GetNodesAtResolution(10):
        # only write/compare compressed data or otherwise tests take too long
        writer.WritePointsCompressed(
            reader.GetPointDataCompressed(node), node.point_count
        )
        num_pts += node.point_count

    writer.Close()

    # validate
    new_reader = copc.LazReader(file_path)
    points = new_reader.GetPoints()
    assert len(points) == num_pts


def test_write():
    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")
    cfg = copc.LazConfigWriter(6)
    writer = copc.LazWriter(file_path, cfg)

    points = copc.Points(cfg.las_header)
    p = points.CreatePoint()
    p.x = 0
    p.y = 0
    p.z = 0
    points.AddPoint(p)
    p = points.CreatePoint()
    p.x = 11
    p.y = 12
    p.z = 13
    points.AddPoint(p)

    writer.WritePoints(points)
    writer.WritePoints(points)
    writer.Close()

    reader = copc.LazReader(file_path)
    read_points = reader.GetPoints()
    assert len(read_points) == 4
    assert read_points.x == [0, 11, 0, 11]
    assert read_points.y == [0, 12, 0, 12]
    assert read_points.z == [0, 13, 0, 13]
