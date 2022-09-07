import sys

import copclib as copc
import pytest


def pack_and_unpack(p, scale, offset):
    points = copc.Points(p.point_format_id, p.EbByteSize())
    points.AddPoint(p)
    return copc.Points.Unpack(points.Pack(scale, offset), p.point_format_id, p.EbByteSize(), scale, offset)[0]

def test_constructor():
    point = copc.Points(
        point_format_id=6,
        eb_byte_size=0,
    ).CreatePoint()

    assert point.HasRgb() is False
    assert point.HasNir() is False

    point_ext = copc.Points(
        8
    ).CreatePoint()

    assert point_ext.HasRgb() is True
    assert point_ext.HasNir() is True


def test_point():
    point6 = copc.Points(
        6
    ).CreatePoint()
    # Position
    point6.x = 2147483647
    point6.y = 2147483647
    point6.z = 2147483647
    assert point6.x == 2147483647
    assert point6.y == 2147483647
    assert point6.z == 2147483647

    # intensity
    point6.intensity = 65535
    assert point6.intensity == 65535

    # Return BitField
    point6.returns_bit_field = 255
    assert point6.returns_bit_field == 255

    # Return Number
    point6.return_number = 0
    assert point6.return_number == 0
    point6.return_number = 15
    assert point6.return_number == 15
    with pytest.raises(RuntimeError):
        point6.return_number = 16

    # Number return
    point6.number_of_returns = 0
    assert point6.number_of_returns == 0
    point6.number_of_returns = 15
    assert point6.number_of_returns == 15
    with pytest.raises(RuntimeError):
        point6.number_of_returns = 16

    # Flags
    point6.flags_bit_field = 255
    assert point6.flags_bit_field == 255

    # synthetic
    point6.synthetic = False
    assert point6.synthetic is False
    point6.synthetic = True
    assert point6.synthetic is True

    # key_point
    point6.key_point = False
    assert point6.key_point is False
    point6.key_point = True
    assert point6.key_point is True

    # withheld
    point6.withheld = False
    assert point6.withheld is False
    point6.withheld = True
    assert point6.withheld is True

    # overlap
    point6.overlap = False
    assert point6.overlap is False
    point6.overlap = True
    assert point6.overlap is True

    # Scanner Channel
    point6.scanner_channel = 0
    assert point6.scanner_channel == 0
    point6.scanner_channel = 3
    assert point6.scanner_channel == 3
    with pytest.raises(RuntimeError):
        point6.scanner_channel = 4

    # Scan Direction
    point6.scan_direction_flag = True
    assert point6.scan_direction_flag is True
    point6.scan_direction_flag = False
    assert point6.scan_direction_flag is False

    # Edge of Flight Line
    point6.edge_of_flight_line = True
    assert point6.edge_of_flight_line is True
    point6.edge_of_flight_line = False
    assert point6.edge_of_flight_line is False

    # classification
    point6.classification = 255
    assert point6.classification == 255
    point6.classification = 0
    assert point6.classification == 0

    # Scan Angle
    point6.scan_angle = -30000
    assert point6.scan_angle == -30000
    assert point6.scan_angle_degrees == pytest.approx(-180.0)
    point6.scan_angle = 30000
    assert point6.scan_angle == 30000
    assert point6.scan_angle_degrees == pytest.approx(180.0)

    # User Data
    point6.user_data = 255
    assert point6.user_data == 255

    # Point Source ID
    point6.point_source_id = 65535
    assert point6.point_source_id == 65535

    # GPS Time
    point6.gps_time = sys.float_info.max
    assert point6.gps_time == sys.float_info.max

    # Point Record Length
    assert point6.point_record_length == 30

    # Checks
    with pytest.raises(RuntimeError):
        point6.rgb = [65535, 65535, 65535]
        assert point6.red
        assert point6.green
        assert point6.blue
        point6.nir = 65535
        assert point6.nir
        assert point6.scan_angleRank
        point6.scan_angleRank = 127
        point6.ReturnsScanDirEofBitFields = 255
        assert point6.ReturnsScanDirEofBitFields
        point6.classificationBitFields = 255
        assert point6.classificationBitFields

    # ToString
    str(point6)

    point7 = copc.Points(
        7
    ).CreatePoint()

    point7.rgb = [65535, 65535, 65535]
    assert point7.red == 65535
    assert point7.green == 65535
    assert point7.blue == 65535
    assert point7.point_record_length == 36

    with pytest.raises(RuntimeError):
        point7.nir = 65535
        assert point7.nir

    point8 = copc.Points(
        8
    ).CreatePoint()

    point8.nir = 65535
    assert point8.nir == 65535
    assert point8.point_record_length == 38


def test_point_conversion():
    point = copc.Points(
        6
    ).CreatePoint()

    assert point.gps_time == 0
    assert point.scanner_channel == 0
    assert point.overlap == 0
    assert point.gps_time == 0

    point.ToPointFormat(7)
    assert point.gps_time == 0
    assert point.scanner_channel == 0
    assert point.overlap == 0
    assert point.gps_time == 0
    assert point.red == 0
    assert point.green == 0
    assert point.blue == 0

    point.ToPointFormat(8)
    assert point.gps_time == 0
    assert point.scanner_channel == 0
    assert point.overlap == 0
    assert point.gps_time == 0
    assert point.red == 0
    assert point.green == 0
    assert point.blue == 0
    assert point.nir == 0


def test_operators_equal():
    # Format 0
    point = copc.Points(
        6
    ).CreatePoint()
    point_other = copc.Points(
        6
    ).CreatePoint()
    assert point == point_other

    # Format 7
    point_other.ToPointFormat(7)
    assert point != point_other
    point.ToPointFormat(7)
    assert point == point_other

    # Format 8
    point_other.ToPointFormat(8)
    assert point != point_other
    point.ToPointFormat(8)
    assert point == point_other

    # Atributes

    point.x = 4
    assert point != point_other
    point_other.x = 4
    assert point == point_other

    point.y = 4
    assert point != point_other
    point_other.y = 4
    assert point == point_other

    point.z = 4
    assert point != point_other
    point_other.z = 4
    assert point == point_other

    point.intensity = 4
    assert point != point_other
    point_other.intensity = 4
    assert point == point_other

    point.return_number = 4
    assert point != point_other
    point_other.return_number = 4
    assert point == point_other

    point.number_of_returns = 4
    assert point != point_other
    point_other.number_of_returns = 4
    assert point == point_other

    point.classification = 4
    assert point != point_other
    point_other.classification = 4
    assert point == point_other

    point.scan_direction_flag = True
    assert point != point_other
    point_other.scan_direction_flag = True
    assert point == point_other

    point.edge_of_flight_line = True
    assert point != point_other
    point_other.edge_of_flight_line = True
    assert point == point_other

    point.synthetic = True
    assert point != point_other
    point_other.synthetic = True
    assert point == point_other

    point.key_point = True
    assert point != point_other
    point_other.key_point = True
    assert point == point_other

    point.withheld = True
    assert point != point_other
    point_other.withheld = True
    assert point == point_other

    point.overlap = True
    assert point != point_other
    point_other.overlap = True
    assert point == point_other

    point.scanner_channel = 2
    assert point != point_other
    point_other.scanner_channel = 2
    assert point == point_other

    point.user_data = 4
    assert point != point_other
    point_other.user_data = 4
    assert point == point_other

    point.point_source_id = 4
    assert point != point_other
    point_other.point_source_id = 4
    assert point == point_other

    point.gps_time = 4.0
    assert point != point_other
    point_other.gps_time = 4.0
    assert point == point_other

    point.red = 4
    assert point != point_other
    point_other.red = 4
    assert point == point_other

    point.green = 4
    assert point != point_other
    point_other.green = 4
    assert point == point_other

    point.blue = 4
    assert point != point_other
    point_other.blue = 4
    assert point == point_other

    point.nir = 4
    assert point != point_other
    point_other.nir = 4
    assert point == point_other


def test_extra_byte():
    point = copc.Points(
        6
    ).CreatePoint()
    assert point.point_format_id == 6
    assert point.point_record_length == 30
    with pytest.raises(RuntimeError):
        point.extra_bytes = [2, 3, 4]
    assert len(point.extra_bytes) == 0
    assert point.EbByteSize() == 0

    point = copc.Points(
        6,
        eb_byte_size=5,
    ).CreatePoint()
    assert point.point_format_id == 6
    assert point.point_record_length == 30 + 5
    assert point.EbByteSize() == 5
    assert len(point.extra_bytes) == 5
    with pytest.raises(RuntimeError):
        point.extra_bytes = [2, 3, 4]
    point.extra_bytes = [2, 3, 4, 5, 6]
    assert point.extra_bytes == [2, 3, 4, 5, 6]


def test_operator_copy():
    point = copc.Points(
        8,
        eb_byte_size=2,
    ).CreatePoint()

    point.x = 4
    point.y = 4
    point.z = 4

    point.gps_time = 4.0
    point.red = 4
    point.nir = 4

    point.extra_bytes = [2, 5]

    point_other = point

    assert point == point_other


def test_scaled_xyz():

    pfid = 8

    # No scale and offset
    point = copc.Points(pfid).CreatePoint()

    point.x = 4
    point.y = 4.5
    point.z = -0.1

    test_point = pack_and_unpack(point, [1,1,1], [0,0,0])
    assert test_point.x == 4
    assert test_point.y == 5
    assert test_point.z == 0

    # Scale test
    point = copc.Points(
        pfid
    ).CreatePoint()

    point.x = 0.01
    point.y = 0.02
    point.z = 0.03

    test_point = pack_and_unpack(point, [1,1,1], [0,0,0])
    assert test_point.x == 0
    assert test_point.y == 0
    assert test_point.z == 0

    test_point = pack_and_unpack(point, [0.01,0.01,0.01], [0,0,0])

    assert test_point.x == 0.01
    assert test_point.y == 0.02
    assert test_point.z == 0.03

    # Offset test

    scale = [1, 1, 1]
    offset = copc.Vector3([50001.456, 4443.123, -255.001])
    point = copc.Points(pfid).CreatePoint()

    point.x = 50502.888
    point.y = 4002.111
    point.z = -80.5

    test_point = pack_and_unpack(point, scale, offset)

    assert test_point.x == pytest.approx(50502.456, 0.000001)
    assert test_point.y == pytest.approx(4002.123, 0.000001)
    assert test_point.z == pytest.approx(-80.001, 0.000001)

    # Scale and Offset test
    scale=[0.001, 0.001, 0.001]
    offset=[50001.456, 4443.123, -255.001]
    point = copc.Points(pfid).CreatePoint()

    point.x = 50502.888
    point.y = 4002.111
    point.z = -80.5

    test_point = pack_and_unpack(point, scale, offset)
    assert test_point.x == pytest.approx(50502.888, 0.000001)
    assert test_point.y == pytest.approx(4002.111, 0.000001)
    assert test_point.z == pytest.approx(-80.5, 0.000001)

    # Change scale test
    offset = [50001.456, 4443.123, -255.001]
    scale = [0.001, 0.001, 0.001]
    new_scale = [1,1,1]
    point = copc.Points(pfid).CreatePoint()

    point.x = 50502.888
    point.y = 4002.111
    point.z = -80.5

    test_point = pack_and_unpack(point, new_scale, offset)
    assert test_point.x == pytest.approx(50502.456, 0.000001)
    assert test_point.y == pytest.approx(4002.123, 0.000001)
    assert test_point.z == pytest.approx(-80.001, 0.000001)


    # Precision checks
    points = copc.Points(pfid)
    point = points.CreatePoint()
    point.x = 50501132.888123
    points.AddPoint(point)
    with pytest.raises(RuntimeError):
        points.Pack([0.000001, 0.000001, 0.000001], [0, 0, 0])


    points = copc.Points(pfid)
    point = points.CreatePoint()
    point.x = 0
    points.AddPoint(point)
    with pytest.raises(RuntimeError):
        points.Pack([1, 1, 1], [-8001100065, 0, 0])


def test_within():
    point = copc.Points(6).CreatePoint()

    point.x = 5
    point.y = 5
    point.z = 5

    assert point.Within(copc.Box.MaxBox())
    assert not point.Within(copc.Box.EmptyBox())

    # 2D box
    assert point.Within(copc.Box(0, 0, 5, 5))
    assert not point.Within(copc.Box(6, 0, 10, 5))

    # 3D box
    assert point.Within(copc.Box(0, 0, 0, 5, 5, 5))
    assert not point.Within(copc.Box(6, 0, 0, 10, 5, 5))
