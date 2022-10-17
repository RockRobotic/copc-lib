import copclib as copc
import pytest

from .utils import get_autzen_file


def test_reader():
    # Given a valid file path
    reader = copc.LazReader(get_autzen_file())

    # Given an invalid file path
    with pytest.raises(RuntimeError):
        assert copc.LazReader("invalid_path/non_existant_file.copc.laz")

    # LasHeader Test
    header = reader.laz_config.las_header
    assert header.point_format_id == 7
    assert header.point_count == 10653336
    assert header.point_record_length == 36
    assert header.EbByteSize() == 0

    # WKT Test
    wkt = reader.laz_config.wkt
    assert len(wkt) == 0
