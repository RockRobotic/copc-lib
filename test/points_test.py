import copclib


def test_constructor():
    points = copclib.Points(3, 4)
    assert points.PointFormatID() == 3
    # assert points.PointRecordLength() == 38

    points = copclib.Points(3, 4, 10)
    assert points.PointFormatID() == 3
    # assert points.PointRecordLength() == 38
    assert len(points.GetPoints()) == 10
