import pickle
import copclib as copc


def test_get_bounds():
    reader = copc.FileReader("../test/data/autzen-classified.copc.laz")
    las_header = reader.GetLasHeader()
    box = las_header.GetBounds()
    assert box.x_min == las_header.min.x
    assert box.y_min == las_header.min.y
    assert box.z_min == las_header.min.z
    assert box.x_max == las_header.max.x
    assert box.y_max == las_header.max.y
    assert box.z_max == las_header.max.z
