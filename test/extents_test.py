import pytest
from sys import float_info
import copclib as copc


def test_copc_extents():

    point_format_id = 7
    num_extra_bytes = 5

    #### Constructor ####

    # Empty Constructor

    extents = copc.CopcExtents(point_format_id, num_extra_bytes)
    assert extents.point_format_id == point_format_id
    assert len(extents.extra_bytes) == num_extra_bytes

    # Point format checks
    with pytest.raises(RuntimeError):
        copc.CopcExtents(5)
        copc.CopcExtents(9)

    #### Default Extents ####

    file_path = "../test/data/writer_test.copc.laz"

    cfg = copc.LasHeaderConfig(7)
    eb_vlr = copc.EbVlr(1)
    cfg.extra_bytes = eb_vlr
    writer = copc.FileWriter(file_path, cfg, 256)

    extents = writer.extents
    assert extents.point_format_id == 7
    assert len(extents.extra_bytes) == 1

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.extents

    assert extents.point_format_id == 7
    assert len(extents.extra_bytes) == 1

    assert extents.x.minimum == 0
    assert extents.y.minimum == 0

    for extent in extents.extents:
        assert extent.minimum == 0
        assert extent.maximum == 0

    #### Default Extents ####
    writer = copc.FileWriter(file_path, cfg, 256)

    extents = writer.extents

    extents.x.minimum = -float_info.max
    extents.x.maximum = float_info.max

    extents.extra_bytes[0].minimum = -float_info.max
    extents.extra_bytes[0].maximum = float_info.max

    writer.extents = extents

    writer.Close()

    reader = copc.FileReader(file_path)

    extents = reader.extents

    assert extents.x.minimum == -float_info.max
    assert extents.x.maximum == float_info.max

    assert extents.extra_bytes[0].minimum == -float_info.max
    assert extents.extra_bytes[0].maximum == float_info.max

    #### SetCopcExtents ####
    extents = copc.CopcExtents(point_format_id, num_extra_bytes)

    extent_vec = [
        (1, 1)
        for _ in range(
            copc.CopcExtents.NumberOfExtents(point_format_id, num_extra_bytes)
        )
    ]

    extents.extents = extent_vec

    for extent in extents.extents:
        assert extent.minimum == 1
        assert extent.maximum == 1

    # Test checks on size
    with pytest.raises(RuntimeError):
        extents.extents = extent_vec[:-1]

    extent_vec.append((1, 1))
    with pytest.raises(RuntimeError):
        extents.extents = extent_vec
