import copclib as copc
import pytest


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
        points.AddPoint()
        
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
        points.AddPoint(p)

    assert points.Size == num_points
    assert len(points) == num_points

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
        p = points[i]
        assert p.X == i * 50 + 8
        assert p.Y == i + 800
        assert p.Z == i * 4

    # test last point
    last_point = points[-1]
    assert last_point.X == 1
    assert last_point.Y == 2
    assert last_point.Z == 3


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
        
    assert min(points.X) == 0
    assert max(points.X) == num_points - 1
    assert min(points.Y) == 0
    assert max(points.Y) == (num_points - 1) * 3
    assert min(points.Z) == 0 - 80
    assert max(points.Z) == (num_points - 1) - 80

    # test index setter
    for i in range(len(points)):
        p = points[i]
        p.X = 20
        p.Y = 30
        p.Z = 40

    assert min(points.X) == 20
    assert max(points.X) == 20
    assert min(points.Y) == 30
    assert max(points.Y) == 30
    assert min(points.Z) == 40
    assert max(points.Z) == 40

    # test iterator setter
    for p in points:
        p.X = -50
        p.Y = -60
        p.Z = -70

    assert min(points.X) == -50
    assert max(points.X) == -50
    assert min(points.Y) == -60
    assert max(points.Y) == -60
    assert min(points.Z) == -70
    assert max(points.Z) == -70
 
        
def test_points_indexer_setter():
    points = copc.Points(
        3, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        points.AddPoint(points.CreatePoint())
        
    assert min(points.X) == 0
    assert max(points.X) == 0
    assert min(points.Y) == 0
    assert max(points.Y) == 0
    assert min(points.Z) == 0
    assert max(points.Z) == 0

    points[0].X = 20
    points[0].Y = 40
    points[0].Z = 80
    points[2].Intensity = 60000

    assert points[0].X == 20
    assert points[0].Y == 40
    assert points[0].Z == 80
    assert points[2].Intensity == 60000