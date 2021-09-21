import pickle
import copclib as copc


def test_las_header():

    reader = copc.FileReader("../test/data/autzen-classified.copc.laz")
    las_header = reader.GetLasHeader()

    assert las_header.file_source_id == las_header.file_source_id
    assert las_header.global_encoding == las_header.global_encoding
    assert las_header.guid == las_header.guid
    assert las_header.version_major == las_header.version_major
    assert las_header.version_minor == las_header.version_minor
    assert las_header.system_identifier == las_header.system_identifier
    assert las_header.generating_software == las_header.generating_software
    assert las_header.creation_day == las_header.creation_day
    assert las_header.creation_year == las_header.creation_year
    assert las_header.header_size == las_header.header_size
    assert las_header.point_offset == las_header.point_offset
    assert las_header.vlr_count == las_header.vlr_count
    assert las_header.point_format_id == las_header.point_format_id
    assert las_header.point_record_length == las_header.point_record_length
    assert las_header.point_count == las_header.point_count
    assert las_header.points_by_return == las_header.points_by_return
    assert las_header.scale.x == las_header.scale.x
    assert las_header.scale.y == las_header.scale.y
    assert las_header.scale.z == las_header.scale.z
    assert las_header.offset.x == las_header.offset.x
    assert las_header.offset.y == las_header.offset.y
    assert las_header.offset.z == las_header.offset.z
    assert las_header.max == las_header.max
    assert las_header.min == las_header.min
    assert las_header.wave_offset == las_header.wave_offset
    assert las_header.evlr_offset == las_header.evlr_offset
    assert las_header.evlr_count == las_header.evlr_count
    assert las_header.points_by_return_14 == las_header.points_by_return_14


def test_pickling():

    reader = copc.FileReader("../test/data/autzen-classified.copc.laz")
    las_header = reader.GetLasHeader()

    data = pickle.dumps(las_header, -1)
    las_header2 = pickle.loads(data)

    assert las_header.file_source_id == las_header2.file_source_id
    assert las_header.global_encoding == las_header2.global_encoding
    assert las_header.guid == las_header2.guid
    assert las_header.version_major == las_header2.version_major
    assert las_header.version_minor == las_header2.version_minor
    assert las_header.system_identifier == las_header2.system_identifier
    assert las_header.generating_software == las_header2.generating_software
    assert las_header.creation_day == las_header2.creation_day
    assert las_header.creation_year == las_header2.creation_year
    assert las_header.header_size == las_header2.header_size
    assert las_header.point_offset == las_header2.point_offset
    assert las_header.vlr_count == las_header2.vlr_count
    assert las_header.point_format_id == las_header2.point_format_id
    assert las_header.point_record_length == las_header2.point_record_length
    assert las_header.point_count == las_header2.point_count
    assert las_header.points_by_return == las_header2.points_by_return
    assert las_header.scale == las_header2.scale
    assert las_header.offset == las_header2.offset
    assert las_header.max == las_header2.max
    assert las_header.min == las_header2.min
    assert las_header.wave_offset == las_header2.wave_offset
    assert las_header.evlr_offset == las_header2.evlr_offset
    assert las_header.evlr_count == las_header2.evlr_count
    assert las_header.point_count_14 == las_header2.point_count_14
    assert las_header.points_by_return_14 == las_header2.points_by_return_14
