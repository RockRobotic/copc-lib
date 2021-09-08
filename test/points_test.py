import copclib


def test_constructor():
    points = copclib.Points(3, 4)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38

    points = copclib.Points(3, 4, 10)
    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert len(points.GetPoints()) == 10

    point1 = copclib.Point(3, 4)
    point1.X(11)
    point1.Y(11)
    point1.Z(11)

    points = copclib.Points(3, 4, [point1, copclib.Point(3, 4), copclib.Point(3, 4)])

    assert points.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    for point in points.GetPoints():
        assert point.PointFormatID() == 3
    assert points.PointRecordLength() == 38
    assert points.GetPoints()[0].X() == 11
    assert points.GetPoints()[0].Y() == 11
    assert points.GetPoints()[0].Z() == 11
