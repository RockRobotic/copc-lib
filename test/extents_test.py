import pytest
from sys import float_info
import copclib as copc


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

    file_path = "writer_test.copc.laz"

    eb_vlr = copc.EbVlr(num_eb_items)
    cfg = copc.CopcConfig(point_format_id, extra_bytes_vlr=eb_vlr)
    writer = copc.FileWriter(file_path, cfg)

    extents = writer.copc_extents
    assert extents.point_format_id == 7
    assert len(extents.extra_bytes) == 3

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.copc_extents

    assert extents.point_format_id == 7
    assert len(extents.extra_bytes) == 3

    assert extents.classification.minimum == 0
    assert extents.intensity.minimum == 0

    for extent in extents.extents:
        assert extent.minimum == 0
        assert extent.maximum == 0

    #### Set Extents ####
    writer = copc.FileWriter(file_path, cfg)

    extents = writer.copc_extents

    extents.classification.minimum = -float_info.max
    extents.classification.maximum = float_info.max

    extents.intensity = (-10, 5)  # Tuple implicit conversion

    extents.extra_bytes[0].minimum = -float_info.max
    extents.extra_bytes[0].maximum = float_info.max

    writer.copc_extents = extents

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.copc_extents

    assert extents.classification.minimum == -float_info.max
    assert extents.classification.maximum == float_info.max

    assert extents.intensity.minimum == -10
    assert extents.intensity.maximum == 5

    assert extents.extra_bytes[0].minimum == -float_info.max
    assert extents.extra_bytes[0].maximum == float_info.max

    #### Extra byte list setter ####
    writer = copc.FileWriter(file_path, cfg)
    extents = writer.copc_extents

    with pytest.raises(RuntimeError):
        extents.extra_bytes = [(0, 1), (2, 3)]
        extents.extra_bytes = [(0, 1), (2, 3), (4, 5), (6, 7)]

    extents.extra_bytes = [(0, 1), (2, 3), (4, 5)]

    writer.copc_extents = extents

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.copc_extents

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

    extents.extents = [(2, 2) for _ in range(len(extents.extents))]

    for extent in extents.extents:
        assert extent.minimum == 2
        assert extent.maximum == 2
