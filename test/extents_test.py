import pytest
from sys import float_info
import copclib as copc
from .utils import get_data_dir
import os


def test_copc_extents():

    point_format_id = 7
    num_eb_items = 3

    #### Constructor ####

    # Empty Constructor

    extents = copc.CopcExtents(point_format_id, num_eb_items)
    assert extents.point_format_id == point_format_id
    assert len(extents.extra_bytes) == num_eb_items

    # Point format checks
    with pytest.raises(RuntimeError):
        copc.CopcExtents(5)
        copc.CopcExtents(9)

    #### Default Extents ####

    file_path = os.path.join(get_data_dir(), "writer_test.copc.laz")

    eb_vlr = copc.EbVlr(num_eb_items)
    cfg = copc.CopcConfigWriter(point_format_id, extra_bytes_vlr=eb_vlr)
    writer = copc.FileWriter(file_path, cfg)

    extents = writer.copc_config.copc_extents
    assert extents.point_format_id == 7
    assert len(extents.extra_bytes) == 3

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.copc_config.copc_extents

    assert extents.point_format_id == 7
    assert extents.has_extended_stats == False
    assert len(extents.extra_bytes) == 3

    assert extents.classification.minimum == 0
    assert extents.intensity.minimum == 0

    for extent in extents.extents:
        assert extent.minimum == 0
        assert extent.maximum == 0

    #### Set Extents ####
    cfg = copc.CopcConfigWriter(
        point_format_id, extra_bytes_vlr=eb_vlr, has_extended_stats=True
    )
    writer = copc.FileWriter(file_path, cfg)

    extents = writer.copc_config.copc_extents

    assert extents.has_extended_stats == True

    extents.x.minimum = 1
    extents.x.maximum = 2
    extents.x.mean = 3
    extents.x.var = 4
    extents.y.minimum = 5
    extents.y.maximum = 6
    extents.y.mean = 7
    extents.y.var = 8
    extents.z.minimum = 9
    extents.z.maximum = 10
    extents.z.mean = 11
    extents.z.var = 12

    extents.classification.minimum = -float_info.max
    extents.classification.maximum = float_info.max
    extents.classification.mean = 15
    extents.classification.var = 5

    extents.intensity = (-10, 5)  # Tuple implicit conversion

    extents.extra_bytes[0].minimum = -float_info.max
    extents.extra_bytes[0].maximum = float_info.max
    extents.extra_bytes[0].mean = 566
    extents.extra_bytes[0].var = 158

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.copc_config.copc_extents

    assert extents.x.minimum == 1
    assert extents.x.maximum == 2
    assert extents.x.mean == 3
    assert extents.x.var == 4
    assert extents.y.minimum == 5
    assert extents.y.maximum == 6
    assert extents.y.mean == 7
    assert extents.y.var == 8
    assert extents.z.minimum == 9
    assert extents.z.maximum == 10
    assert extents.z.mean == 11
    assert extents.z.var == 12

    assert extents.classification.minimum == -float_info.max
    assert extents.classification.maximum == float_info.max
    assert extents.classification.mean == 15
    assert extents.classification.var == 5

    assert extents.intensity == (-10, 5, 0, 1)  # Tuple implicit conversion
    assert extents.intensity == (-10, 5)  # Tuple implicit conversion

    assert extents.extra_bytes[0].minimum == -float_info.max
    assert extents.extra_bytes[0].maximum == float_info.max
    assert extents.extra_bytes[0].mean == 566
    assert extents.extra_bytes[0].var == 158

    #### Extra byte list setter ####
    writer = copc.FileWriter(file_path, cfg)
    extents = writer.copc_config.copc_extents

    with pytest.raises(RuntimeError):
        extents.extra_bytes = [(0, 1), (2, 3)]
        extents.extra_bytes = [(0, 1), (2, 3), (4, 5), (6, 7)]

    extents.extra_bytes = [(0, 1), (2, 3), (4, 5)]

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.copc_config.copc_extents

    assert extents.extra_bytes[0].minimum == 0
    assert extents.extra_bytes[0].maximum == 1

    assert extents.extra_bytes[1].minimum == 2
    assert extents.extra_bytes[1].maximum == 3

    assert extents.extra_bytes[2].minimum == 4
    assert extents.extra_bytes[2].maximum == 5

    #### Get/Set Extents List ####
    extents = copc.CopcExtents(point_format_id, num_eb_items)

    for extent in extents.extents:
        extent.minimum = 1
        extent.maximum = 1

    for extent in extents.extents:
        assert extent.minimum == 1
        assert extent.maximum == 1
