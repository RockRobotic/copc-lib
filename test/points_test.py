import copclib as copc
import pytest
import random


def test_points_constructor():
    points = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )
    assert points.point_format_id == 6
    assert points.point_record_length == 34
    assert len(points) == 0

    point1 = copc.Points(
        6,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=4,
    ).CreatePoint()
    point1.unscaled_x = 11
    point1.unscaled_y = 11
    point1.unscaled_z = 11

    point_list = [
        point1,
        copc.Points(
            6,
            copc.Vector3.DefaultScale(),
            copc.Vector3.DefaultOffset(),
            eb_byte_size=4,
        ).CreatePoint(),
        copc.Points(
            6,
            copc.Vector3.DefaultScale(),
            copc.Vector3.DefaultOffset(),
            eb_byte_size=4,
        ).CreatePoint(),
    ]

    points = copc.Points(point_list)
    assert points.point_format_id == 6
    assert points.point_record_length == 34
    for point in points:
        assert point.point_format_id == 6
    assert points[0].unscaled_y == 11
    assert points[0].unscaled_z == 11

    str(points)


def test_adding_point_to_points():
    points = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 0
    )
    point = copc.Points(
        6,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=0,
    ).CreatePoint()
    point.unscaled_x = 11
    point.unscaled_y = 11
    point.unscaled_z = 11

    points.AddPoint(point)

    assert len(points) == 1
    assert points[0].unscaled_x == 11
    assert points[0].unscaled_y == 11
    assert points[0].unscaled_z == 11

    point = copc.Points(
        6,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=0,
    ).CreatePoint()
    point.unscaled_x = 22
    point.unscaled_y = 22
    point.unscaled_z = 22

    points.AddPoint(point)
    assert len(points) == 2
    assert points[1].unscaled_x == 22
    assert points[1].unscaled_y == 22
    assert points[1].unscaled_z == 22

    # Test check on point format
    point = copc.Points(
        7,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=0,
    ).CreatePoint()
    with pytest.raises(RuntimeError):
        points.AddPoint(point)

    # Test check on extra bytes
    point = copc.Points(
        6,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
        eb_byte_size=1,
    ).CreatePoint()
    with pytest.raises(RuntimeError):
        points.AddPoint(point)


def test_adding_points_to_points():
    points = copc.Points(
        [
            copc.Points(
                6,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                eb_byte_size=4,
            ).CreatePoint()
            for _ in range(10)
        ]
    )
    points_other = copc.Points(
        [
            copc.Points(
                6,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                eb_byte_size=4,
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
                7,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                eb_byte_size=4,
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
                6,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                eb_byte_size=1,
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
                6,
                copc.Vector3.DefaultScale(),
                copc.Vector3.DefaultOffset(),
                eb_byte_size=4,
            ).CreatePoint()
            for _ in range(10)
        ]
    )
    points.ToPointFormat(7)

    assert points.point_format_id == 7
    assert points.point_record_length == 40
    assert points[0].point_format_id == 7
    assert points[0].point_record_length == 40

    with pytest.raises(RuntimeError):
        points.ToPointFormat(5)
    with pytest.raises(RuntimeError):
        points.ToPointFormat(9)


def test_points_iterator():
    points = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        p = points.CreatePoint()
        p.classification = i % 32
        points.AddPoint(p)

    classification_index = [points[i].classification for i in range(num_points)]
    classification_iterator = [p.classification for p in points]

    for i, clas in enumerate(classification_index):
        assert clas == i % 32

    assert classification_index == classification_iterator


def test_points_group_accessors():
    points = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        p = points.CreatePoint()
        p.x = i
        p.y = i * 3
        p.z = i - 80
        points.AddPoint(p)

    assert len(points) == num_points

    # test that the getters work
    for i in range(num_points):
        assert points.x[i] == i
        assert points.y[i] == i * 3
        assert points.z[i] == i - 80

    # generate vector of coordinates
    xn = []
    yn = []
    zn = []
    with pytest.raises(RuntimeError):
        points.x = xn
        points.y = yn
        points.z = zn

    for i in range(num_points - 1):
        xn.append(i * 50 + 8)
        yn.append(i + 800)
        zn.append(i * 4)

    with pytest.raises(RuntimeError):
        points.x = xn
        points.y = yn
        points.z = zn

    # add the last point
    xn.append(1)
    yn.append(2)
    zn.append(3)

    # test setters
    points.x = xn
    points.y = yn
    points.z = zn

    for i in range(num_points - 1):
        p = points[i]
        assert p.x == i * 50 + 8
        assert p.y == i + 800
        assert p.z == i * 4

    # test negative indices
    last_point = points[-1]
    assert last_point.x == 1
    assert last_point.y == 2
    assert last_point.z == 3


def test_within():

    points = copc.Points(6, (1, 1, 1), copc.Vector3.DefaultOffset())

    # generate points
    for i in range(2000):
        p = points.CreatePoint()
        p.x = random.uniform(0, 5)
        p.y = random.uniform(0, 5)
        p.z = random.uniform(0, 5)
        points.AddPoint(p)

    assert points.Within(copc.Box(0, 0, 0, 5, 5, 5))

    p = points.CreatePoint()
    p.x = random.uniform(0, 5)
    p.y = random.uniform(0, 5)
    p.z = 6
    points.AddPoint(p)

    assert not points.Within(copc.Box(0, 0, 0, 5, 5, 5))


def test_get_within():

    points = copc.Points(6, (1, 1, 1), copc.Vector3.DefaultOffset())

    # generate points
    for i in range(2000):
        p = points.CreatePoint()
        p.x = random.uniform(0, 5)
        p.y = random.uniform(0, 5)
        p.z = random.uniform(0, 5)
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
        p.x = i
        p.y = i * 3
        p.z = i - 80
        p.classification = i * 255 // num_points
        p.point_source_id = i * 255 // num_points
        points.AddPoint(p)

    for i, (x, y, z, classification, point_source_id) in enumerate(
        zip(points.x, points.y, points.z, points.classification, points.point_source_id)
    ):
        assert x == i
        assert y == i * 3
        assert z == i - 80
        assert classification == i * 255 // num_points
        assert point_source_id == i * 255 // num_points

    # test slice
    assert points.x[5:10] == [x for x in range(5, 10)]
    assert points.y[-10:] == [x * 3 for x in range(1990, 2000)]
    assert points.z[:10] == [x - 80 for x in range(0, 10)]
    assert points.classification[:10] == [x * 255 // num_points for x in range(0, 10)]
    assert points.point_source_id[:10] == [x * 255 // num_points for x in range(0, 10)]

    # test index setter
    for i in range(len(points)):
        p = points[i]
        p.x = 20
        p.y = 30
        p.z = 40
        p.classification = 50
        p.point_source_id = 50

    assert all([x == 20 for x in points.x])
    assert all([y == 30 for y in points.y])
    assert all([z == 40 for z in points.z])
    assert all([classification == 50 for classification in points.classification])
    assert all([point_source_id == 50 for point_source_id in points.point_source_id])

    # test iterator setter
    for p in points:
        p.x = -50
        p.y = -60
        p.z = -70

    assert all([x == -50 for x in points.x])
    assert all([y == -60 for y in points.y])
    assert all([z == -70 for z in points.z])


def test_points_indexer_setter():
    points = copc.Points(
        6, copc.Vector3.DefaultScale(), copc.Vector3.DefaultOffset(), 4
    )

    # generate points
    num_points = 2000
    for i in range(num_points):
        points.AddPoint(points.CreatePoint())

    assert all([x == 0 for x in points.x])
    assert all([y == 0 for y in points.y])
    assert all([z == 0 for z in points.z])

    points[0].x = 20
    points[0].y = 40
    points[0].z = 80
    points[2].intensity = 60000

    assert points[0].x == 20
    assert points[0].y == 40
    assert points[0].z == 80
    assert points[2].intensity == 60000

    # test slicing setters
    points[:].x = [2] * len(points)
    points[:].y = [4] * len(points)
    points[:].z = [8] * len(points)

    assert all([x == 2 for x in points.x])
    assert all([x == 4 for x in points.y])
    assert all([x == 8 for x in points.z])

    # TODO: Allow the user to set all values the same
    points[1000:].x = [-2] * 1000
    points[1500:1600].y = [-4] * 100
    points[-5:].z = [-8] * 5

    assert all([x == -2 for i, x in enumerate(points.x) if i >= 1000])
    assert all([x != -2 for i, x in enumerate(points.x) if not i >= 1000])
    assert all([x == -4 for i, x in enumerate(points.y) if i >= 1500 and i < 1600])
    assert all(
        [x != -4 for i, x in enumerate(points.y) if not (i >= 1500 and i < 1600)]
    )
    assert all([x == -8 for i, x in enumerate(points.z) if i >= len(points) - 5])
    assert all([x != -8 for i, x in enumerate(points.z) if not (i >= len(points) - 5)])
