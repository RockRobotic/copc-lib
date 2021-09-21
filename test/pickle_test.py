import pickle
import copclib as copc


def test_vector_char():
    char_vec = copc.VectorChar()
    char_vec.append("t")
    char_vec.append("e")
    char_vec.append("s")
    char_vec.append("t")

    char_vec_other = pickle.loads(pickle.dumps(char_vec, -1))

    assert char_vec == char_vec_other


def test_vector3():
    vec3 = copc.Vector3(0, 1, 2)

    vec3_other = pickle.loads(pickle.dumps(vec3, -1))

    assert vec3 == vec3_other


def test_las_header():

    reader = copc.FileReader("../test/data/autzen-classified.copc.laz")
    las_header = reader.GetLasHeader()

    las_header_other = pickle.loads(pickle.dumps(las_header, -1))

    assert las_header.file_source_id == las_header_other.file_source_id
    assert las_header.global_encoding == las_header_other.global_encoding
    assert las_header.guid == las_header_other.guid
    assert las_header.version_major == las_header_other.version_major
    assert las_header.version_minor == las_header_other.version_minor
    assert las_header.system_identifier == las_header_other.system_identifier
    assert las_header.generating_software == las_header_other.generating_software
    assert las_header.creation_day == las_header_other.creation_day
    assert las_header.creation_year == las_header_other.creation_year
    assert las_header.header_size == las_header_other.header_size
    assert las_header.point_offset == las_header_other.point_offset
    assert las_header.vlr_count == las_header_other.vlr_count
    assert las_header.point_format_id == las_header_other.point_format_id
    assert las_header.point_record_length == las_header_other.point_record_length
    assert las_header.point_count == las_header_other.point_count
    assert las_header.points_by_return == las_header_other.points_by_return
    assert las_header.scale == las_header_other.scale
    assert las_header.offset == las_header_other.offset
    assert las_header.max == las_header_other.max
    assert las_header.min == las_header_other.min
    assert las_header.wave_offset == las_header_other.wave_offset
    assert las_header.evlr_offset == las_header_other.evlr_offset
    assert las_header.evlr_count == las_header_other.evlr_count
    assert las_header.point_count_14 == las_header_other.point_count_14
    assert las_header.points_by_return_14 == las_header_other.points_by_return_14
