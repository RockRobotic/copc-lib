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
