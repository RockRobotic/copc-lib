import copclib as copc
import pytest
import random


def test_points_constructor():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )
    assert points.PointFormatID == 3
    assert points.PointRecordLength == 38
    assert len(points.Get()) == 0

    point1 = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), num_extra_bytes=4
    ).CreatePoint()
    point1.UnscaledX = 11
    point1.UnscaledY = 11
    point1.UnscaledZ = 11

    point_list = [
        point1,
        copc.Points(
            3,
            copc.Vector3.DefaultScale(),
            copc.Vector3.DefaultOffset(),
            num_extra_bytes=4,
        ).CreatePoint(),
        copc.Points(
            3,
            copc.Vector3.DefaultScale(),
            copc.Vector3.DefaultOffset(),
            num_extra_bytes=4,
        ).CreatePoint(),
    ]

    points = copc.Points(point_list)
    assert points.PointFormatID == 3
    assert points.PointRecordLength == 38
    for point in points.Get():
        assert point.PointFormatID == 3
    assert points.PointRecordLength == 38
    assert points.Get(idx=0).UnscaledX == 11
    assert points.Get(0).UnscaledY == 11
    assert points.Get(0).UnscaledZ == 11

    str(points)


def test_adding_point_to_points():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 0
    )
    point = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), num_extra_bytes=0
    ).CreatePoint()
    point.UnscaledX = 11
    point.UnscaledY = 11
    point.UnscaledZ = 11

    points.AddPoint(point)

    assert len(points.Get()) == 1
    assert points.Get(0).UnscaledX == 11
    assert points.Get(0).UnscaledY == 11
    assert points.Get(0).UnscaledZ == 11

    point = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), num_extra_bytes=0
    ).CreatePoint()
    point.UnscaledX = 22
    point.UnscaledY = 22
    point.UnscaledZ = 22

    points.AddPoint(point)
    assert len(points.Get()) == 2
    assert points.Get(1).UnscaledX == 22
    assert points.Get(1).UnscaledY == 22
    assert points.Get(1).UnscaledZ == 22

    # Test check on point format
    point = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), num_extra_bytes=0
    ).CreatePoint()
    with pytest.raises(RuntimeError):
        points.AddPoint(point)

    # Test check on extra bytes
    point = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), num_extra_bytes=1
    ).CreatePoint()
    with pytest.raises(RuntimeError):
        points.AddPoint(point)


def test_adding_points_to_points():
    points = copc.Points(
        [
            copc.Points(
                3,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                num_extra_bytes=4,
            ).CreatePoint()
            for _ in range(10)
        ]
    )
    points_other = copc.Points(
        [
            copc.Points(
                3,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                num_extra_bytes=4,
            ).CreatePoint()
            for _ in range(10)
        ]
    )

    points.AddPoints(points_other)

    assert len(points.Get()) == 20

    # Test check on point format
    points_other = copc.Points(
        [
            copc.Points(
                6,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                num_extra_bytes=4,
            ).CreatePoint()
            for _ in range(10)
        ]
    )
    with pytest.raises(RuntimeError):
        points.AddPoints(points_other)

    # Test check on extra bytes
    points_other = copc.Points(
        [
            copc.Points(
                3,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                num_extra_bytes=1,
            ).CreatePoint()
            for _ in range(10)
        ]
    )
    with pytest.raises(RuntimeError):
        points.AddPoints(points_other)


def test_points_format_conversion():
    points = copc.Points(
        [
            copc.Points(
                3,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                num_extra_bytes=4,
            ).CreatePoint()
            for _ in range(10)
        ]
    )
    points.ToPointFormat(6)

    assert points.PointFormatID == 6
    assert points.PointRecordLength == 38
    assert points.Get(0).PointFormatID == 6
    assert points.Get(0).PointRecordLength == 34

    with pytest.raises(RuntimeError):
        points.ToPointFormat(-1)
    with pytest.raises(RuntimeError):
        points.ToPointFormat(11)


def test_points_accessors():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        p = points.CreatePoint()
        p.X = i
        p.Y = i * 3
        p.Z = i - 80
        points.AddPoint(p)

    assert points.Size == num_points

    # test that the getters work
    for i in range(num_points):
        assert points.X[i] == i
        assert points.Y[i] == i * 3
        assert points.Z[i] == i - 80

    # generate vector of coordinates
    Xn = []
    Yn = []
    Zn = []
    with pytest.raises(RuntimeError):
        points.X = Xn
        points.Y = Yn
        points.Z = Zn

    for i in range(num_points - 1):
        Xn.append(i * 50 + 8)
        Yn.append(i + 800)
        Zn.append(i * 4)

    with pytest.raises(RuntimeError):
        points.X = Xn
        points.Y = Yn
        points.Z = Zn

    # add the last point
    Xn.append(1)
    Yn.append(2)
    Zn.append(3)

    # test setters
    points.X = Xn
    points.Y = Yn
    points.Z = Zn

    for i in range(num_points - 1):
        p = points.Get(i)
        assert p.X == i * 50 + 8
        assert p.Y == i + 800
        assert p.Z == i * 4

    # test last point
    last_point = points.Get(points.Size - 1)
    assert last_point.X == 1
    assert last_point.Y == 2
    assert last_point.Z == 3


def test_within():

    points = copc.Points(3, (1, 1, 1), copc.Vector3.DefaultOffset())

    # generate points
    for i in range(2000):
        p = points.CreatePoint()
        p.X = random.uniform(0, 5)
        p.Y = random.uniform(0, 5)
        p.Z = random.uniform(0, 5)
        points.AddPoint(p)

    assert points.Within(copc.Box(0, 5, 0, 5, 0, 5))

    p = points.CreatePoint()
    p.X = random.uniform(0, 5)
    p.Y = random.uniform(0, 5)
    p.Z = 6
    points.AddPoint(p)

    assert not points.Within(copc.Box(0, 5, 0, 5, 0, 5))
