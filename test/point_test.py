import sys

import copclib as copc
import pytest


def test_constructor():
    point = copc.Points(
        point_format_id=6,
        scale=copc.Vector3.DefaultScale(),
        offset=copc.Vector3.DefaultOffset(),
        eb_byte_size=0,
    ).CreatePoint()

    assert point.HasRGB is False
    assert point.HasNIR is False

    point_ext = copc.Points(
        8, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()

    assert point_ext.HasRGB is True
    assert point_ext.HasNIR is True


def test_point():
    point6 = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()
    # Position
    point6.UnscaledX = 2147483647
    point6.UnscaledY = 2147483647
    point6.UnscaledZ = 2147483647
    assert point6.UnscaledX == 2147483647
    assert point6.UnscaledY == 2147483647
    assert point6.UnscaledZ == 2147483647

    # Intensity
    point6.Intensity = 65535
    assert point6.Intensity == 65535

    # Return BitField
    point6.ReturnsBitField = 255
    assert point6.ReturnsBitField == 255

    # Return Number
    point6.ReturnNumber = 0
    assert point6.ReturnNumber == 0
    point6.ReturnNumber = 15
    assert point6.ReturnNumber == 15
    with pytest.raises(RuntimeError):
        point6.ReturnNumber = 16

    # Number return
    point6.NumberOfReturns = 0
    assert point6.NumberOfReturns == 0
    point6.NumberOfReturns = 15
    assert point6.NumberOfReturns == 15
    with pytest.raises(RuntimeError):
        point6.NumberOfReturns = 16

    # Flags
    point6.FlagsBitField = 255
    assert point6.FlagsBitField == 255

    # Synthetic
    point6.Synthetic = False
    assert point6.Synthetic is False
    point6.Synthetic = True
    assert point6.Synthetic is True

    # KeyPoint
    point6.KeyPoint = False
    assert point6.KeyPoint is False
    point6.KeyPoint = True
    assert point6.KeyPoint is True

    # Withheld
    point6.Withheld = False
    assert point6.Withheld is False
    point6.Withheld = True
    assert point6.Withheld is True

    # Overlap
    point6.Overlap = False
    assert point6.Overlap is False
    point6.Overlap = True
    assert point6.Overlap is True

    # Scanner Channel
    point6.ScannerChannel = 0
    assert point6.ScannerChannel == 0
    point6.ScannerChannel = 3
    assert point6.ScannerChannel == 3
    with pytest.raises(RuntimeError):
        point6.ScannerChannel = 4

    # Scan Direction
    point6.ScanDirectionFlag = True
    assert point6.ScanDirectionFlag is True
    point6.ScanDirectionFlag = False
    assert point6.ScanDirectionFlag is False

    # Edge of Flight Line
    point6.EdgeOfFlightLineFlag = True
    assert point6.EdgeOfFlightLineFlag is True
    point6.EdgeOfFlightLineFlag = False
    assert point6.EdgeOfFlightLineFlag is False

    # Classification
    point6.Classification = 255
    assert point6.Classification == 255
    point6.Classification = 0
    assert point6.Classification == 0

    # Scan Angle
    point6.ScanAngle = -30000
    assert point6.ScanAngle == -30000
    assert point6.ScanAngleFloat == -180.0
    point6.ScanAngle = 30000
    assert point6.ScanAngle == 30000
    assert point6.ScanAngleFloat == 180.0
    with pytest.raises(RuntimeError):
        point6.ScanAngle = 30001

    # User Data
    point6.UserData = 255
    assert point6.UserData == 255

    # Point Source ID
    point6.PointSourceID = 65535
    assert point6.PointSourceID == 65535

    # GPS Time
    point6.GPSTime = sys.float_info.max
    assert point6.GPSTime == sys.float_info.max

    # Point Record Length
    assert point6.PointRecordLength == 30

    # Checks
    with pytest.raises(RuntimeError):
        point6.RGB = [65535, 65535, 65535]
        assert point6.Red
        assert point6.Green
        assert point6.Blue
        point6.NIR = 65535
        assert point6.NIR
        assert point6.ScanAngleRank
        point6.ScanAngleRank = 127
        point6.ReturnsScanDirEofBitFields = 255
        assert point6.ReturnsScanDirEofBitFields
        point6.ClassificationBitFields = 255
        assert point6.ClassificationBitFields

    # ToString
    str(point6)

    point7 = copc.Points(
        7, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()

    point7.RGB = [65535, 65535, 65535]
    assert point7.Red == 65535
    assert point7.Green == 65535
    assert point7.Blue == 65535
    assert point7.PointRecordLength == 36

    with pytest.raises(RuntimeError):
        point7.NIR = 65535
        assert point7.NIR

    point8 = copc.Points(
        8, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()

    point8.NIR = 65535
    assert point8.NIR == 65535
    assert point8.PointRecordLength == 38


def test_point_conversion():
    point = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()

    assert point.GPSTime == 0
    assert point.ScannerChannel == 0
    assert point.Overlap == 0
    assert point.GPSTime == 0

    point.ToPointFormat(7)
    assert point.GPSTime == 0
    assert point.ScannerChannel == 0
    assert point.Overlap == 0
    assert point.GPSTime == 0
    assert point.Red == 0
    assert point.Green == 0
    assert point.Blue == 0

    point.ToPointFormat(8)
    assert point.GPSTime == 0
    assert point.ScannerChannel == 0
    assert point.Overlap == 0
    assert point.GPSTime == 0
    assert point.Red == 0
    assert point.Green == 0
    assert point.Blue == 0
    assert point.NIR == 0


def test_operators_equal():
    # Format 0
    point = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()
    point_other = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
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

    point.UnscaledX = 4
    assert point != point_other
    point_other.UnscaledX = 4
    assert point == point_other

    point.UnscaledY = 4
    assert point != point_other
    point_other.UnscaledY = 4
    assert point == point_other

    point.UnscaledZ = 4
    assert point != point_other
    point_other.UnscaledZ = 4
    assert point == point_other

    point.Intensity = 4
    assert point != point_other
    point_other.Intensity = 4
    assert point == point_other

    point.ReturnNumber = 4
    assert point != point_other
    point_other.ReturnNumber = 4
    assert point == point_other

    point.NumberOfReturns = 4
    assert point != point_other
    point_other.NumberOfReturns = 4
    assert point == point_other

    point.Classification = 4
    assert point != point_other
    point_other.Classification = 4
    assert point == point_other

    point.ScanDirectionFlag = True
    assert point != point_other
    point_other.ScanDirectionFlag = True
    assert point == point_other

    point.EdgeOfFlightLineFlag = True
    assert point != point_other
    point_other.EdgeOfFlightLineFlag = True
    assert point == point_other

    point.Synthetic = True
    assert point != point_other
    point_other.Synthetic = True
    assert point == point_other

    point.KeyPoint = True
    assert point != point_other
    point_other.KeyPoint = True
    assert point == point_other

    point.Withheld = True
    assert point != point_other
    point_other.Withheld = True
    assert point == point_other

    point.Overlap = True
    assert point != point_other
    point_other.Overlap = True
    assert point == point_other

    point.ScannerChannel = 2
    assert point != point_other
    point_other.ScannerChannel = 2
    assert point == point_other

    point.UserData = 4
    assert point != point_other
    point_other.UserData = 4
    assert point == point_other

    point.PointSourceID = 4
    assert point != point_other
    point_other.PointSourceID = 4
    assert point == point_other

    point.GPSTime = 4.0
    assert point != point_other
    point_other.GPSTime = 4.0
    assert point == point_other

    point.Red = 4
    assert point != point_other
    point_other.Red = 4
    assert point == point_other

    point.Green = 4
    assert point != point_other
    point_other.Green = 4
    assert point == point_other

    point.Blue = 4
    assert point != point_other
    point_other.Blue = 4
    assert point == point_other

    point.NIR = 4
    assert point != point_other
    point_other.NIR = 4
    assert point == point_other


def test_extra_byte():
    point = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()
    assert point.PointFormatID == 6
    assert point.PointRecordLength == 30
    with pytest.raises(RuntimeError):
        point.ExtraBytes = [2, 3, 4]
    assert len(point.ExtraBytes) == 0
    assert point.EbByteSize == 0

    point = copc.Points(
        6,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=5,
    ).CreatePoint()
    assert point.PointFormatID == 6
    assert point.PointRecordLength == 30 + 5
    assert point.EbByteSize == 5
    assert len(point.ExtraBytes) == 5
    with pytest.raises(RuntimeError):
        point.ExtraBytes = [2, 3, 4]
    point.ExtraBytes = [2, 3, 4, 5, 6]
    assert point.ExtraBytes == [2, 3, 4, 5, 6]


def test_operator_copy():
    point = copc.Points(
        8,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=2,
    ).CreatePoint()

    point.UnscaledX = 4
    point.UnscaledY = 4
    point.UnscaledZ = 4

    point.GPSTime = 4.0
    point.Red = 4
    point.NIR = 4

    point.ExtraBytes = [2, 5]

    point_other = point

    assert point == point_other


def test_scaled_xyz():

    pfid = 8

    # No scale and offset
    point = copc.Points(pfid, scale=[1, 1, 1], offset=[0, 0, 0]).CreatePoint()

    point.UnscaledX = 4
    point.UnscaledY = 4
    point.UnscaledZ = 4

    assert point.X == 4
    assert point.Y == 4
    assert point.Z == 4

    point.X = 5
    point.Y = 6
    point.Z = 7

    assert point.UnscaledX == 5
    assert point.UnscaledY == 6
    assert point.UnscaledZ == 7

    # Scale test
    point = copc.Points(
        pfid, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset()
    ).CreatePoint()

    point.UnscaledX = 1
    point.UnscaledY = 2
    point.UnscaledZ = 3

    assert point.X == 0.01
    assert point.Y == 0.02
    assert point.Z == 0.03

    point.X = 200
    point.Y = 300
    point.Z = 400

    assert point.UnscaledX == 200 * 100
    assert point.UnscaledY == 300 * 100
    assert point.UnscaledZ == 400 * 100
    # Offset test

    scale = [1, 1, 1]
    offset = copc.Vector3([50001.456, 4443.123, -255.001])
    point = copc.Points(pfid, scale, offset).CreatePoint()

    point.UnscaledX = 0
    point.UnscaledY = -800
    point.UnscaledZ = 3

    assert point.X == 0 + offset.x
    assert point.Y == -800 + offset.y
    assert point.Z == 3 + offset.z

    point.X = 50502.888
    point.Y = 4002.111
    point.Z = -80.5

    assert point.UnscaledX == 501  # 50502.888 - 50001.456 = 501.432
    assert point.UnscaledY == -441
    assert point.UnscaledZ == 175  # -80.5 - -255.001 = 255.001 - 80.5 = 175

    # (value * scale) + offset
    assert point.X == pytest.approx(50502.456, 0.000001)
    assert point.Y == pytest.approx(4002.123, 0.000001)
    assert point.Z == pytest.approx(-80.001, 0.000001)

    # Scale and Offset test
    point = copc.Points(
        pfid, scale=[0.001, 0.001, 0.001], offset=[50001.456, 4443.123, -255.001]
    ).CreatePoint()

    point.UnscaledX = 0
    point.UnscaledY = -800
    point.UnscaledZ = 300532

    assert point.X == pytest.approx(50001.456, 0.000001)
    assert point.Y == pytest.approx(4442.323, 0.000001)
    assert point.Z == pytest.approx(45.531, 0.000001)

    point.X = 50502.888
    point.Y = 4002.111
    point.Z = -80.5

    assert point.UnscaledX == 501432
    assert point.UnscaledY == -441012
    assert point.UnscaledZ == 174501

    # (value * scale) + offset
    assert point.X == pytest.approx(50502.888, 0.000001)
    assert point.Y == pytest.approx(4002.111, 0.000001)
    assert point.Z == pytest.approx(-80.5, 0.000001)

    # Precision checks
    point = copc.Points(pfid, [0.000001, 0.000001, 0.000001], [0, 0, 0]).CreatePoint()

    with pytest.raises(RuntimeError):
        point.X = 50501132.888123

    point = copc.Points(pfid, [1, 1, 1], [-8001100065, 0, 0]).CreatePoint()
    with pytest.raises(RuntimeError):
        point.X = 0


def test_within():
    point = copc.Points(6, (1, 1, 1), copc.Vector3.DefaultOffset()).CreatePoint()

    point.X = 5
    point.Y = 5
    point.Z = 5

    assert point.Within(copc.Box.MaxBox())
    assert not point.Within(copc.Box.ZeroBox())

    # 2D box
    assert point.Within(copc.Box(0, 0, 5, 5))
    assert not point.Within(copc.Box(6, 0, 10, 5))

    # 3D box
    assert point.Within(copc.Box(0, 0, 0, 5, 5, 5))
    assert not point.Within(copc.Box(6, 0, 0, 10, 5, 5))
