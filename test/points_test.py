import copclib as copc
import pytest
import random


def test_points_constructor():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )
    assert points.PointFormatID == 3
    assert points.PointRecordLength == 38
    assert len(points) == 0

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
    for point in points:
        assert point.PointFormatID == 3
    assert points.PointRecordLength == 38
    assert points[0].UnscaledY == 11
    assert points[0].UnscaledZ == 11

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

    assert len(points) == 1
    assert points[0].UnscaledX == 11
    assert points[0].UnscaledY == 11
    assert points[0].UnscaledZ == 11

    point = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), num_extra_bytes=0
    ).CreatePoint()
    point.UnscaledX = 22
    point.UnscaledY = 22
    point.UnscaledZ = 22

    points.AddPoint(point)
    assert len(points) == 2
    assert points[1].UnscaledX == 22
    assert points[1].UnscaledY == 22
    assert points[1].UnscaledZ == 22

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

    assert len(points) == 20

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
    assert points[0].PointFormatID == 6
    assert points[0].PointRecordLength == 34

    with pytest.raises(RuntimeError):
        points.ToPointFormat(-1)
    with pytest.raises(RuntimeError):
        points.ToPointFormat(11)


def test_points_iterator():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        p = points.CreatePoint()
        p.Classification = i % 32
        points.AddPoint(p)

    classification_index = [points[i].Classification for i in range(num_points)]
    classification_iterator = [p.Classification for p in points]

    for i, clas in enumerate(classification_index):
        assert clas == i % 32

    assert classification_index == classification_iterator


def test_points_group_accessors():
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
        p.Classification = i * 255 / num_points
        p.PointSourceID = i * 255 / num_points
        p.Red = i * 4
        p.Green = i * 5
        p.Blue = i * 6
        points.AddPoint(p)

    assert len(points) == num_points

    # test that the getters work
    for i in range(num_points):
        assert points.X[i] == i
        assert points.Y[i] == i * 3
        assert points.Z[i] == i - 80
        assert points.Classification[i] == i * 255 / num_points
        assert points.PointSourceID[i] == i * 255 / num_points
        assert points.Red[i] == i * 4
        assert points.Green[i] == i * 5
        assert points.Blue[i] == i * 6

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
        p = points[i]
        assert p.X == i * 50 + 8
        assert p.Y == i + 800
        assert p.Z == i * 4

    # test negative indices
    last_point = points[-1]
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

    assert points.Within(copc.Box(0, 0, 0, 5, 5, 5))

    p = points.CreatePoint()
    p.X = random.uniform(0, 5)
    p.Y = random.uniform(0, 5)
    p.Z = 6
    points.AddPoint(p)

    assert not points.Within(copc.Box(0, 0, 0, 5, 5, 5))


def test_get_within():

    points = copc.Points(3, (1, 1, 1), copc.Vector3.DefaultOffset())

    # generate points
    for i in range(2000):
        p = points.CreatePoint()
        p.X = random.uniform(0, 5)
        p.Y = random.uniform(0, 5)
        p.Z = random.uniform(0, 5)
        points.AddPoint(p)

    box = copc.Box(0, 0, 0, 2.5, 2.5, 2.5)
    points = points.GetWithin(box)
    assert copc.Points(points).Within(box)


def test_points_accessors():
    points = copc.Points(
        7, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        p = points.CreatePoint()
        p.X = i
        p.Y = i * 3
        p.Z = i - 80
        p.Classification = i * 255 // num_points
        p.PointSourceID = i * 255 // num_points
        points.AddPoint(p)

    for i, (x, y, z, classification, point_source_id) in enumerate(
        zip(points.X, points.Y, points.Z, points.Classification, points.PointSourceID)
    ):
        assert x == i
        assert y == i * 3
        assert z == i - 80
        assert classification == i * 255 // num_points
        assert point_source_id == i * 255 // num_points

    # test slice
    assert points.X[5:10] == [x for x in range(5, 10)]
    assert points.Y[-10:] == [x * 3 for x in range(1990, 2000)]
    assert points.Z[:10] == [x - 80 for x in range(0, 10)]
    assert points.Classification[:10] == [x * 255 // num_points for x in range(0, 10)]
    assert points.PointSourceID[:10] == [x * 255 // num_points for x in range(0, 10)]

    # test index setter
    for i in range(len(points)):
        p = points[i]
        p.X = 20
        p.Y = 30
        p.Z = 40
        p.Classification = 50
        p.PointSourceID = 50

    assert all([x == 20 for x in points.X])
    assert all([y == 30 for y in points.Y])
    assert all([z == 40 for z in points.Z])
    assert all([classification == 50 for classification in points.Classification])
    assert all([point_source_id == 50 for point_source_id in points.PointSourceID])

    # test iterator setter
    for p in points:
        p.X = -50
        p.Y = -60
        p.Z = -70

    assert all([x == -50 for x in points.X])
    assert all([y == -60 for y in points.Y])
    assert all([z == -70 for z in points.Z])


def test_points_indexer_setter():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        points.AddPoint(points.CreatePoint())

    assert all([x == 0 for x in points.X])
    assert all([y == 0 for y in points.Y])
    assert all([z == 0 for z in points.Z])

    points[0].X = 20
    points[0].Y = 40
    points[0].Z = 80
    points[2].Intensity = 60000

    assert points[0].X == 20
    assert points[0].Y == 40
    assert points[0].Z == 80
    assert points[2].Intensity == 60000

    # test slicing setters
    points[:].X = [2] * len(points)
    points[:].Y = [4] * len(points)
    points[:].Z = [8] * len(points)

    assert all([x == 2 for x in points.X])
    assert all([x == 4 for x in points.Y])
    assert all([x == 8 for x in points.Z])

    # TODO: Allow the user to set all values the same
    points[1000:].X = [-2] * 1000
    points[1500:1600].Y = [-4] * 100
    points[-5:].Z = [-8] * 5

    assert all([x == -2 for i, x in enumerate(points.X) if i >= 1000])
    assert all([x != -2 for i, x in enumerate(points.X) if not i >= 1000])
    assert all([x == -4 for i, x in enumerate(points.Y) if i >= 1500 and i < 1600])
    assert all(
        [x != -4 for i, x in enumerate(points.Y) if not (i >= 1500 and i < 1600)]
    )
    assert all([x == -8 for i, x in enumerate(points.Z) if i >= len(points) - 5])
    assert all([x != -8 for i, x in enumerate(points.Z) if not (i >= len(points) - 5)])
