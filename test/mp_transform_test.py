import os
import sys

from .utils import generate_test_file, get_data_dir

import copclib as copc
from copclib.mp.transform import transform_multithreaded
import numpy as np
import pytest


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_copc_copy():
    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")
    reader = copc.FileReader(generate_test_file())
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    transform_multithreaded(reader, writer)
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


def _transform_fun(offset, points, **kwargs):
    xyz = np.stack([points.x, points.y, points.z], axis=1)
    xyz += offset

    points.x = xyz[:, 0]
    points.y = xyz[:, 1]
    points.z = xyz[:, 2]

    return points


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_translate():
    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")
    reader = copc.FileReader(generate_test_file())
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    offset = [50, 30, 20]

    transform_multithreaded(
        reader,
        writer,
        _transform_fun,
        transform_function_args=dict(offset=offset),
        update_minmax=True,
    )
    writer.Close()

    # validate
    new_reader = copc.FileReader(file_path)

    old_header_min = reader.copc_config.las_header.min
    old_header_min = np.array([old_header_min.x, old_header_min.y, old_header_min.z])
    new_header_min = new_reader.copc_config.las_header.min
    new_header_min = np.array([new_header_min.x, new_header_min.y, new_header_min.z])

    np.testing.assert_allclose(old_header_min + offset, new_header_min, atol=0.01)

    for node in reader.GetAllNodes():
        assert node.IsValid()
        new_node = new_reader.FindNode(node.key)
        assert new_node.IsValid()
        assert new_node.key == node.key
        assert new_node.point_count == node.point_count
        assert new_node.byte_size == node.byte_size


def _transform_fun_empty(writer_header, **kwargs):
    new_points = copc.Points(writer_header)
    return new_points


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_translate_empty():
    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")
    reader = copc.FileReader(generate_test_file())
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    transform_multithreaded(
        reader,
        writer,
        _transform_fun_empty,
        update_minmax=True,
    )
    writer.Close()

    # validate
    new_reader = copc.FileReader(file_path)

    new_header_min = new_reader.copc_config.las_header.min
    new_header_min = np.array([new_header_min.x, new_header_min.y, new_header_min.z])
    np.testing.assert_allclose(new_header_min, [0, 0, 0], atol=0.01)

    new_header_max = new_reader.copc_config.las_header.max
    new_header_max = np.array([new_header_max.x, new_header_max.y, new_header_max.z])
    np.testing.assert_allclose(new_header_max, [0, 0, 0], atol=0.01)

    assert new_reader.copc_config.las_header.point_count == 0
    assert len(new_reader.GetAllPoints()) == 0


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_translate_laz():
    file_path = os.path.join(get_data_dir(), "writer_test.laz")
    reader = copc.FileReader(generate_test_file())
    writer = copc.LazWriter(
        file_path,
        reader.copc_config,
    )

    offset = [40, 30, 20]

    transform_multithreaded(
        reader,
        writer,
        _transform_fun,
        transform_function_args=dict(offset=offset),
        update_minmax=True,
    )
    writer.Close()

    # validate
    new_reader = copc.LazReader(file_path)

    old_header_min = reader.copc_config.las_header.min
    old_header_min = np.array([old_header_min.x, old_header_min.y, old_header_min.z])
    new_header_min = new_reader.laz_config.las_header.min
    new_header_min = np.array([new_header_min.x, new_header_min.y, new_header_min.z])

    np.testing.assert_allclose(old_header_min + offset, new_header_min, atol=0.01)

    points = new_reader.GetPoints()
    assert len(points) == reader.copc_config.las_header.point_count


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_translate_laz_empty():
    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")
    reader = copc.FileReader(generate_test_file())
    writer = copc.LazWriter(
        file_path,
        reader.copc_config,
    )

    transform_multithreaded(
        reader,
        writer,
        _transform_fun_empty,
        update_minmax=True,
    )
    writer.Close()

    # validate
    new_reader = copc.LazReader(file_path)

    new_header_min = new_reader.laz_config.las_header.min
    new_header_min = np.array([new_header_min.x, new_header_min.y, new_header_min.z])
    np.testing.assert_allclose(new_header_min, [0, 0, 0], atol=0.01)

    new_header_max = new_reader.laz_config.las_header.max
    new_header_max = np.array([new_header_max.x, new_header_max.y, new_header_max.z])
    np.testing.assert_allclose(new_header_max, [0, 0, 0], atol=0.01)

    assert new_reader.laz_config.las_header.point_count == 0
    assert len(new_reader.GetPoints()) == 0
