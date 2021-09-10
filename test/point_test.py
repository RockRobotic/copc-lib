import sys

import copclib
import pytest


def test_constructor():
    point = copclib.Point(0)

    assert point.HasExtendedPoint() is False
    assert point.HasGPSTime() is False
    assert point.HasRGB() is False
    assert point.HasNIR() is False

    point_ext = copclib.Point(8, 0)

    assert point_ext.HasExtendedPoint() is True
    assert point_ext.HasGPSTime() is True
    assert point_ext.HasRGB() is True
    assert point_ext.HasNIR() is True


def test_point_format10():
    point0 = copclib.Point(0)
    # Position
    point0.X(2147483647)
    point0.Y(2147483647)
    point0.Z(2147483647)
    assert point0.X() == 2147483647
    assert point0.Y() == 2147483647
    assert point0.Z() == 2147483647

    # Intensity
    point0.Intensity(65535)
    assert point0.Intensity() == 65535

    # Return Number
    point0.ReturnNumber(7)
    assert point0.ReturnNumber() == 7
    with pytest.raises(RuntimeError):
        point0.ReturnNumber(8)

    # Number of Returns
    point0.NumberOfReturns(7)
    assert point0.NumberOfReturns() == 7
    with pytest.raises(RuntimeError):
        point0.NumberOfReturns(8)

    # Scan Direction
    point0.ScanDirectionFlag(True)
    assert point0.ScanDirectionFlag() is True

    # Edge of Flight Line
    point0.EdgeOfFlightLineFlag(True)
    assert point0.EdgeOfFlightLineFlag() is True

    # ReturnsScanDirEofBitField
    point0.ReturnsScanDirEofBitFields(172)
    assert point0.ReturnNumber() == 4
    assert point0.NumberOfReturns() == 5
    assert point0.ScanDirectionFlag() is False
    assert point0.EdgeOfFlightLineFlag() is True

    # Classification
    point0.Classification(31)
    assert point0.Classification() == 31
    with pytest.raises(RuntimeError):
        point0.Classification(32)

    # Synthetic
    point0.Synthetic(True)
    assert point0.Synthetic() is True

    # KeyPoint
    point0.KeyPoint(True)
    assert point0.KeyPoint() is True

    # Withheld
    point0.Withheld(True)
    assert point0.Withheld() is True

    # Classification Bitfield
    point0.ClassificationBitFields(255)
    assert point0.ClassificationBitFields() == 255
    point0.ClassificationBitFields(78)
    assert point0.Classification() == 14
    assert point0.Synthetic() is False
    assert point0.KeyPoint() is True
    assert point0.Withheld() is False

    # Scan Angle
    point0.ScanAngleRank(90)
    assert point0.ScanAngleRank() == 90
    assert point0.ScanAngle() == 90.0
    with pytest.raises(RuntimeError):
        point0.ScanAngleRank(91)
        point0.ScanAngleRank(-91)

    # User Data
    point0.UserData(255)
    assert point0.UserData() == 255

    # Point Source ID
    point0.PointSourceID(65535)
    assert point0.PointSourceID() == 65535

    assert point0.PointRecordLength() == 20

    # Checks
    with pytest.raises(RuntimeError):
        point0.ExtendedFlagsBitFields(0)
        point0.ExtendedFlagsBitFields()
        point0.ScannerChannel(0)
        point0.ScannerChannel()
        point0.NIR(65535)
        point0.NIR()
        point0.Red(65535)
        point0.Red()
        point0.Green(65535)
        point0.Green()
        point0.Blue(65535)
        point0.Blue()
        point0.GPSTime(sys.float_info.max)
        point0.GPSTime()
        point0.ExtendedScanAngle(32767)
        point0.ExtendedScanAngle()
        point0.Overlap(True)
        point0.Overlap()
        point0.ExtendedReturnsBitFields(255)
        point0.ExtendedReturnsBitFields()
        point0.ExtendedFlagsBitFields(255)
        point0.ExtendedFlagsBitFields()

    str(point0)

    point1 = copclib.Point(1)

    point1.GPSTime(sys.float_info.max)
    assert point1.GPSTime() == sys.float_info.max
    assert point1.PointRecordLength() == 28
    with pytest.raises(RuntimeError):
        point0.Red(65535)
        point0.Red()
        point0.Green(65535)
        point0.Green()
        point0.Blue(65535)
        point0.Blue()
        point0.NIR(65535)
        point0.NIR()

    point2 = copclib.Point(2)
    assert point2.PointRecordLength() == 26

    point2.Red(65535)
    assert point2.Red() == 65535
    point2.Green(65535)
    assert point2.Green() == 65535
    point2.Blue(65535)
    assert point2.Blue() == 65535

    point2.RGB(65535, 65535, 65535)
    assert point2.Red() == 65535
    assert point2.Green() == 65535
    assert point2.Blue() == 65535

    with pytest.raises(RuntimeError):
        point2.GPSTime(sys.float_info.max)
        point2.GPSTime()
        point2.NIR(65535)
        point2.NIR()

    point3 = copclib.Point(3)
    assert point3.PointRecordLength() == 34

    point3.GPSTime(sys.float_info.max)
    point3.GPSTime()
    point3.RGB(65535, 65535, 65535)
    point3.Red()
    point3.Green()
    point3.Blue()
    with pytest.raises(RuntimeError):
        point3.NIR(65535)
        point3.NIR()


def test_point_format14():
    point6 = copclib.Point(6)
    # Position
    point6.X(2147483647)
    point6.Y(2147483647)
    point6.Z(2147483647)
    assert point6.X() == 2147483647
    assert point6.Y() == 2147483647
    assert point6.Z() == 2147483647

    # Intensity
    point6.Intensity(65535)
    assert point6.Intensity() == 65535

    # Return BitField
    point6.ExtendedReturnsBitFields(255)
    assert point6.ExtendedReturnsBitFields() == 255

    # Return Number
    point6.ReturnNumber(0)
    assert point6.ReturnNumber() == 0
    point6.ReturnNumber(15)
    assert point6.ReturnNumber() == 15
    with pytest.raises(RuntimeError):
        point6.ReturnNumber(16)

    # Number return
    point6.NumberOfReturns(0)
    assert point6.NumberOfReturns() == 0
    point6.NumberOfReturns(15)
    assert point6.NumberOfReturns() == 15
    with pytest.raises(RuntimeError):
        point6.NumberOfReturns(16)

    # Flags
    point6.ExtendedFlagsBitFields(255)
    assert point6.ExtendedFlagsBitFields() == 255

    # Synthetic
    point6.Synthetic(False)
    assert point6.Synthetic() is False
    point6.Synthetic(True)
    assert point6.Synthetic() is True

    # KeyPoint
    point6.Withheld(False)
    assert point6.Withheld() is False
    point6.Withheld(True)
    assert point6.Withheld() is True

    # Withheld
    point6.Withheld(False)
    assert point6.Withheld() is False
    point6.Withheld(True)
    assert point6.Withheld() is True

    # Overlap
    point6.Overlap(False)
    assert point6.Overlap() is False
    point6.Overlap(True)
    assert point6.Overlap() is True

    # Scanner Channel
    point6.ScannerChannel(0)
    assert point6.ScannerChannel() == 0
    point6.ScannerChannel(3)
    assert point6.ScannerChannel() == 3
    with pytest.raises(RuntimeError):
        point6.ScannerChannel(4)

    # Scan Direction
    point6.ScanDirectionFlag(True)
    assert point6.ScanDirectionFlag() is True
    point6.ScanDirectionFlag(False)
    assert point6.ScanDirectionFlag() is False

    # Edge of Flight Line
    point6.EdgeOfFlightLineFlag(True)
    assert point6.EdgeOfFlightLineFlag() is True
    point6.EdgeOfFlightLineFlag(False)
    assert point6.EdgeOfFlightLineFlag() is False

    # Classification
    point6.Classification(255)
    assert point6.Classification() == 255
    point6.Classification(0)
    assert point6.Classification() == 0

    # Scan Angle
    point6.ExtendedScanAngle(-30000)
    assert point6.ExtendedScanAngle() == -30000
    assert point6.ScanAngle() == -180.0
    with pytest.raises(RuntimeError):
        point6.ExtendedScanAngle(-30001)
    point6.ExtendedScanAngle(30000)
    assert point6.ExtendedScanAngle() == 30000
    assert point6.ScanAngle() == 180.0
    with pytest.raises(RuntimeError):
        point6.ExtendedScanAngle(30001)

    # User Data
    point6.UserData(255)
    assert point6.UserData() == 255

    # Point Source ID
    point6.PointSourceID(65535)
    assert point6.PointSourceID() == 65535

    # GPS Time
    point6.GPSTime(sys.float_info.max)
    assert point6.GPSTime() == sys.float_info.max

    # Point Record Length
    assert point6.PointRecordLength() == 30

    # Checks
    with pytest.raises(RuntimeError):
        point6.RGB(65535, 65535, 65535)
        point6.Red()
        point6.Green()
        point6.Blue()
        point6.NIR(65535)
        point6.NIR()
        point6.ScanAngleRank()
        point6.ScanAngleRank(127)
        point6.ReturnsScanDirEofBitFields(255)
        point6.ReturnsScanDirEofBitFields()
        point6.ClassificationBitFields(255)
        point6.ClassificationBitFields()

    # ToString
    str(point6)

    point7 = copclib.Point(7)

    point7.RGB(65535, 65535, 65535)
    assert point7.Red() == 65535
    assert point7.Green() == 65535
    assert point7.Blue() == 65535
    assert point7.PointRecordLength() == 36

    with pytest.raises(RuntimeError):
        point7.NIR(65535)
        point7.NIR()

    point8 = copclib.Point(8)

    point8.NIR(65535)
    assert point8.NIR() == 65535
    assert point8.PointRecordLength() == 38


def test_point_conversion_10():
    point = copclib.Point(1)

    assert point.GPSTime() == 0

    point.ToPointFormat(2)
    assert point.Red() == 0
    assert point.Green() == 0
    assert point.Blue() == 0

    point.ToPointFormat(3)
    assert point.GPSTime() == 0
    assert point.Red() == 0
    assert point.Green() == 0
    assert point.Blue() == 0


def test_point_conversion_14():
    point = copclib.Point(6)

    assert point.GPSTime() == 0
    assert point.ScannerChannel() == 0
    assert point.Overlap() == 0
    assert point.GPSTime() == 0

    point.ToPointFormat(7)
    assert point.GPSTime() == 0
    assert point.ScannerChannel() == 0
    assert point.Overlap() == 0
    assert point.GPSTime() == 0
    assert point.Red() == 0
    assert point.Green() == 0
    assert point.Blue() == 0

    point.ToPointFormat(8)
    assert point.GPSTime() == 0
    assert point.ScannerChannel() == 0
    assert point.Overlap() == 0
    assert point.GPSTime() == 0
    assert point.Red() == 0
    assert point.Green() == 0
    assert point.Blue() == 0
    assert point.NIR() == 0


def test_point_conversion_10_to_14():
    point = copclib.Point(0)

    point.ReturnNumber(5)
    point.NumberOfReturns(6)
    point.ScanDirectionFlag(False)
    point.EdgeOfFlightLineFlag(True)
    point.Classification(15)
    point.Synthetic(True)
    point.KeyPoint(False)
    point.Withheld(True)
    point.ScanAngleRank(45)

    point.ToPointFormat(6)

    assert point.ReturnNumber() == 5
    assert point.NumberOfReturns() == 6
    assert point.ScanDirectionFlag() is False
    assert point.EdgeOfFlightLineFlag() is True
    assert point.Classification() == 15
    assert point.Synthetic() is True
    assert point.KeyPoint() is False
    assert point.Withheld() is True
    assert point.Overlap() is False
    assert point.ExtendedScanAngle() == 7500
    assert point.ScannerChannel() == 0
    with pytest.raises(RuntimeError):
        point.ReturnsScanDirEofBitFields()

    point.ToPointFormat(0)

    assert point.ReturnNumber() == 5
    assert point.NumberOfReturns() == 6
    assert point.ScanDirectionFlag() is False
    assert point.EdgeOfFlightLineFlag() is True
    assert point.Classification() == 15
    assert point.Synthetic() is True
    assert point.KeyPoint() is False
    assert point.Withheld() is True
    assert point.ScanAngleRank() == 45

    point.ToPointFormat(6)

    point.ReturnNumber(13)
    point.NumberOfReturns(14)
    point.Classification(134)
    point.ScannerChannel(2)
    point.Synthetic(True)
    point.KeyPoint(False)
    point.Withheld(True)
    point.ExtendedScanAngle(28000)

    point.ToPointFormat(0)

    assert point.ReturnNumber() == 7
    assert point.NumberOfReturns() == 7
    assert point.Classification() == 31
    assert point.ScanAngleRank() == 90
    with pytest.raises(RuntimeError):
        point.Overlap()
        point.ScannerChannel()
        point.ExtendedReturnsBitFields()
        point.ExtendedFlagsBitFields()

    point.ToPointFormat(6)

    assert point.ReturnNumber() == 7
    assert point.NumberOfReturns() == 7
    assert point.Classification() == 31
    assert point.ExtendedScanAngle() == 15000
    assert point.Overlap() is False
    assert point.ScannerChannel() == 0


def test_operators_equal():
    # Format 0
    point = copclib.Point(0)
    point_other = copclib.Point(0)

    assert point == point_other

    # Format 1
    point_other.ToPointFormat(1)
    assert point != point_other
    point.ToPointFormat(1)
    assert point == point_other

    # Format 2
    point_other.ToPointFormat(2)
    assert point != point_other
    point.ToPointFormat(2)
    assert point == point_other

    # Format 3
    point_other.ToPointFormat(3)
    assert point != point_other
    point.ToPointFormat(3)
    assert point == point_other

    # Format 6
    point_other.ToPointFormat(6)
    assert point != point_other
    point.ToPointFormat(6)
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

    point.X(4)
    assert point != point_other
    point_other.X(4)
    assert point == point_other

    point.Y(4)
    assert point != point_other
    point_other.Y(4)
    assert point == point_other

    point.Z(4)
    assert point != point_other
    point_other.Z(4)
    assert point == point_other

    point.Intensity(4)
    assert point != point_other
    point_other.Intensity(4)
    assert point == point_other

    point.ReturnNumber(4)
    assert point != point_other
    point_other.ReturnNumber(4)
    assert point == point_other

    point.NumberOfReturns(4)
    assert point != point_other
    point_other.NumberOfReturns(4)
    assert point == point_other

    point.Classification(4)
    assert point != point_other
    point_other.Classification(4)
    assert point == point_other

    point.ScanDirectionFlag(True)
    assert point != point_other
    point_other.ScanDirectionFlag(True)
    assert point == point_other

    point.EdgeOfFlightLineFlag(True)
    assert point != point_other
    point_other.EdgeOfFlightLineFlag(True)
    assert point == point_other

    point.Synthetic(True)
    assert point != point_other
    point_other.Synthetic(True)
    assert point == point_other

    point.KeyPoint(True)
    assert point != point_other
    point_other.KeyPoint(True)
    assert point == point_other

    point.Withheld(True)
    assert point != point_other
    point_other.Withheld(True)
    assert point == point_other

    point.Overlap(True)
    assert point != point_other
    point_other.Overlap(True)
    assert point == point_other

    point.ScannerChannel(2)
    assert point != point_other
    point_other.ScannerChannel(2)
    assert point == point_other

    point.UserData(4)
    assert point != point_other
    point_other.UserData(4)
    assert point == point_other

    point.PointSourceID(4)
    assert point != point_other
    point_other.PointSourceID(4)
    assert point == point_other

    point.GPSTime(4.0)
    assert point != point_other
    point_other.GPSTime(4.0)
    assert point == point_other

    point.Red(4)
    assert point != point_other
    point_other.Red(4)
    assert point == point_other

    point.Green(4)
    assert point != point_other
    point_other.Green(4)
    assert point == point_other

    point.Blue(4)
    assert point != point_other
    point_other.Blue(4)
    assert point == point_other

    point.NIR(4)
    assert point != point_other
    point_other.NIR(4)
    assert point == point_other


def test_extra_byte():
    point = copclib.Point(0)
    assert point.PointFormatID() == 0
    assert point.PointRecordLength() == 20
    with pytest.raises(RuntimeError):
        point.ExtraBytes([2, 3, 4])
    assert len(point.ExtraBytes()) == 0
    assert point.NumExtraBytes() == 0

    point = copclib.Point(0, 5)
    assert point.PointFormatID() == 0
    assert point.PointRecordLength() == 20 + 5
    assert point.NumExtraBytes() == 5
    assert len(point.ExtraBytes()) == 5
    with pytest.raises(RuntimeError):
        point.ExtraBytes([2, 3, 4])
    point.ExtraBytes([2, 3, 4, 5, 6])
    assert point.ExtraBytes() == [2, 3, 4, 5, 6]

    point.ToPointFormat(6)
    assert point.PointFormatID() == 6
    assert point.PointRecordLength() == 30 + 5
    assert point.NumExtraBytes() == 5
    assert len(point.ExtraBytes()) == 5
    assert point.ExtraBytes() == [2, 3, 4, 5, 6]
    with pytest.raises(RuntimeError):
        point.ExtraBytes([2, 3, 4])


def test_operator_copy():
    point = copclib.Point(8, 2)

    point.X(4)
    point.Y(4)
    point.Z(4)

    point.GPSTime(4.0)
    point.Red(4)
    point.NIR(4)

    point.ExtraBytes([2, 5])

    point_other = point

    assert point == point_other
