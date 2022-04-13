import pickle
import copclib as copc

from .utils import get_autzen_file


def test_get_bounds():
    reader = copc.FileReader(get_autzen_file())
    las_header = reader.copc_config.las_header
    box = las_header.Bounds()
    assert box.x_min == las_header.min.x
    assert box.y_min == las_header.min.y
    assert box.z_min == las_header.min.z
    assert box.x_max == las_header.max.x
    assert box.y_max == las_header.max.y
    assert box.z_max == las_header.max.z

    str(las_header)


def test_update_bounds():
    las_header = copc.LasHeader()

    assert las_header.min.x == 0.0
    assert las_header.min.y == 0.0
    assert las_header.min.z == 0.0
    assert las_header.max.x == 0.0
    assert las_header.max.y == 0.0
    assert las_header.max.z == 0.0

    point = copc.Points(las_header).CreatePoint()
    point.x = 10
    point.y = -5
    point.z = 1
    las_header.CheckAndUpdateBounds(point)
    assert las_header.min.x == 0.0
    assert las_header.min.y == -5.0
    assert las_header.min.z == 0.0
    assert las_header.max.x == 10.0
    assert las_header.max.y == 0.0
    assert las_header.max.z == 1.0

    point.x = -10
    point.y = 5
    point.z = -1
    las_header.CheckAndUpdateBounds(point)
    assert las_header.min.x == -10.0
    assert las_header.min.y == -5.0
    assert las_header.min.z == -1.0
    assert las_header.max.x == 10.0
    assert las_header.max.y == 5.0
    assert las_header.max.z == 1.0
