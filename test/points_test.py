import copclib
import pytest


def test_points_constructor():
    points = copclib.Points(3, 4)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert len(points.GetPoints()) == 0

    points = copclib.Points(3, 4, 10)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert len(points.GetPoints()) == 10
    for point in points.GetPoints():
        assert point.PointFormatID() == 3

    point1 = copclib.Point(3, 4)
    point1.X(11)
    point1.Y(11)
    point1.Z(11)

    point_list = [point1, copclib.Point(3, 4), copclib.Point(3, 4)]

    points = copclib.Points(3, 4, point_list)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    for point in points.GetPoints():
        assert point.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert points.GetPoints()[0].X() == 11
    assert points.GetPoints()[0].Y() == 11
    assert points.GetPoints()[0].Z() == 11

    # Test check on constant point format
    point4 = copclib.Point(6, 4)
    point_list.append(point4)
    with pytest.raises(RuntimeError):
        copclib.Points(3, 4, point_list)

    point_list = point_list[:-1]
    point5 = copclib.Point(3, 2)
    point_list.append(point5)
    with pytest.raises(RuntimeError):
        copclib.Points(3, 4, point_list)


def test_adding_point_to_points():
    points = copclib.Points(3, 0)
    point = copclib.Point(3, 0)
    point.X(11)
    point.Y(11)
    point.Z(11)

    points.AddPoint(point)

    assert len(points.GetPoints()) == 1
    assert points.GetPoints()[0].X() == 11
    assert points.GetPoints()[0].Y() == 11
    assert points.GetPoints()[0].Z() == 11

    point = copclib.Point(3, 0)
    point.X(22)
    point.Y(22)
    point.Z(22)

    points.AddPoint(point)
    assert len(points.GetPoints()) == 2
    assert points.GetPoints()[1].X() == 22
    assert points.GetPoints()[1].Y() == 22
    assert points.GetPoints()[1].Z() == 22

    # Test check on point format
    point = copclib.Point(6, 0)
    with pytest.raises(RuntimeError):
        points.AddPoint(point)

    # Test check on extra bytes
    point = copclib.Point(3, 1)
    with pytest.raises(RuntimeError):
        points.AddPoint(point)


def test_adding_points_to_points():
    points = copclib.Points(3, 4, [copclib.Point(3, 4) for i in range(10)])
    points_other = copclib.Points(3, 4, [copclib.Point(3, 4) for i in range(10)])

    points.AddPoints(points_other)

    assert len(points.GetPoints()) == 20

    # Test check on point format
    points_other = copclib.Points(6, 4, [copclib.Point(6, 4) for i in range(10)])
    with pytest.raises(RuntimeError):
        points.AddPoints(points_other)

    # Test check on extra bytes
    points_other = copclib.Points(3, 1, [copclib.Point(3, 1) for i in range(10)])
    with pytest.raises(RuntimeError):
        points.AddPoints(points_other)


def test_points_format_conversion():
    points = copclib.Points(3, 4, [copclib.Point(3, 4) for i in range(10)])
    points.ToPointFormat(6)

    assert points.PointFormatID() == 6
    assert points.PointRecordLength() == 38
    assert points.GetPoints()[0].PointFormatID() == 6
    assert points.GetPoints()[0].PointRecordLength() == 34

    with pytest.raises(RuntimeError):
        points.ToPointFormat(-1)
    with pytest.raises(RuntimeError):
        points.ToPointFormat(11)
