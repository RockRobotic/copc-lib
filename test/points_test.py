import copclib as copc
import pytest


def test_points_constructor():
    points = copc.Points(3, 4)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert len(points.Get()) == 0

    point1 = copc.Point(3, 4)
    point1.X(11)
    point1.Y(11)
    point1.Z(11)

    point_list = [point1, copc.Point(3, 4), copc.Point(3, 4)]

    points = copc.Points(point_list)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    for point in points.Get():
        assert point.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert points.Get(0).X() == 11
    assert points.Get(0).Y() == 11
    assert points.Get(0).Z() == 11

    str(points)


def test_adding_point_to_points():
    points = copc.Points(3, 0)
    point = copc.Point(3, 0)
    point.X(11)
    point.Y(11)
    point.Z(11)

    points.AddPoint(point)

    assert len(points.Get()) == 1
    assert points.Get(0).X() == 11
    assert points.Get(0).Y() == 11
    assert points.Get(0).Z() == 11

    point = copc.Point(3, 0)
    point.X(22)
    point.Y(22)
    point.Z(22)

    points.AddPoint(point)
    assert len(points.Get()) == 2
    assert points.Get(1).X() == 22
    assert points.Get(1).Y() == 22
    assert points.Get(1).Z() == 22

    # Test check on point format
    point = copc.Point(6, 0)
    with pytest.raises(RuntimeError):
        points.AddPoint(point)

    # Test check on extra bytes
    point = copc.Point(3, 1)
    with pytest.raises(RuntimeError):
        points.AddPoint(point)


def test_adding_points_to_points():
    points = copc.Points([copc.Point(3, 4) for i in range(10)])
    points_other = copc.Points([copc.Point(3, 4) for i in range(10)])

    points.AddPoints(points_other)

    assert len(points.Get()) == 20

    # Test check on point format
    points_other = copc.Points([copc.Point(6, 4) for i in range(10)])
    with pytest.raises(RuntimeError):
        points.AddPoints(points_other)

    # Test check on extra bytes
    points_other = copc.Points([copc.Point(3, 1) for i in range(10)])
    with pytest.raises(RuntimeError):
        points.AddPoints(points_other)


def test_points_format_conversion():
    points = copc.Points([copc.Point(3, 4) for i in range(10)])
    points.ToPointFormat(6)

    assert points.PointFormatID() == 6
    assert points.PointRecordLength() == 38
    assert points.Get(0).PointFormatID() == 6
    assert points.Get(0).PointRecordLength() == 34

    with pytest.raises(RuntimeError):
        points.ToPointFormat(-1)
    with pytest.raises(RuntimeError):
        points.ToPointFormat(11)
