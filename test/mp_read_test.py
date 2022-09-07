import sys

from .utils import get_autzen_file

import copclib as copc
from copclib.mp.read import read_concat_xyz_class_limit, read_map_xyz_class_limit
import laspy
import pytest
import numpy as np


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_xyz_noclass_limit():
    reader = copc.FileReader(get_autzen_file())
    xyz = read_concat_xyz_class_limit(reader)

    las = laspy.read(get_autzen_file())
    assert len(xyz) == len(las)


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_xyz_class_limit():
    classifications_to_test = [0, 1, 2]

    reader = copc.FileReader(get_autzen_file())

    for classification in classifications_to_test:
        xyz = read_concat_xyz_class_limit(reader, [classification])

        las = laspy.read(get_autzen_file())
        las_points = las.points[las.classification == classification]
        assert len(xyz) == len(las_points)
        print(len(xyz), len(las_points))


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_xyz_map_noclass_limit():
    reader = copc.FileReader(get_autzen_file())
    key_xyz_map = read_map_xyz_class_limit(reader)

    for node in reader.GetAllNodes():
        points = reader.GetPoints(node)
        xyz_real = np.stack([points.x, points.y, points.z], axis=1)
        xyz_test = key_xyz_map[str(node.key)]
        np.testing.assert_allclose(xyz_real, xyz_test)


@pytest.mark.skipif(sys.version_info < (3, 7), reason="requires python3.7")
def test_xyz_map_class_limit():
    classification_limits = [0, 1, 2]

    reader = copc.FileReader(get_autzen_file())

    key_xyz_map = read_map_xyz_class_limit(
        reader, classification_limits=classification_limits
    )

    for node in reader.GetAllNodes():

        points = reader.GetPoints(node)
        point_limit = [p for p in points if p.classification in classification_limits]
        if len(point_limit) == 0:
            assert str(node.key) not in key_xyz_map
        else:
            xyz_test = key_xyz_map[str(node.key)]
            points = copc.Points(point_limit)
            xyz_real = np.stack([points.x, points.y, points.z], axis=1)
            print(xyz_real.shape, xyz_test.shape)
            np.testing.assert_allclose(xyz_real, xyz_test)
