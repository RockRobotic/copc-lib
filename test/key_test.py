from sys import float_info

import copclib as copc


def test_key_validity():
    assert copc.VoxelKey().IsValid() is False
    assert copc.VoxelKey(d=-1, x=-1, y=-1, z=-1).IsValid() is False
    assert copc.VoxelKey(-1, 0, 0, 0).IsValid() is False
    assert copc.VoxelKey(0, -1, 0, 0).IsValid() is False
    assert copc.VoxelKey(0, 0, -1, 0).IsValid() is False
    assert copc.VoxelKey(0, 0, 0, -1).IsValid() is False
    assert copc.VoxelKey(0, 0, 0, 0).IsValid() is True
    assert copc.VoxelKey(1, 1, 1, 1).IsValid() is True


def test_key_operators():
    assert copc.VoxelKey(0, 0, 0, 0) == copc.VoxelKey(0, 0, 0, 0)
    assert copc.VoxelKey(-1, -1, -1, -1) == copc.VoxelKey(-1, -1, -1, -1)
    assert copc.VoxelKey(0, 0, 0, 0) != copc.VoxelKey(1, 1, 1, 1)


def test_get_parent():
    assert copc.VoxelKey(-1, -1, -1, -1).GetParent().IsValid() is False

    assert copc.VoxelKey(4, 4, 6, 12).GetParent() == copc.VoxelKey(3, 2, 3, 6)
    assert copc.VoxelKey(4, 5, 6, 13).GetParent() == copc.VoxelKey(3, 2, 3, 6)
    assert copc.VoxelKey(3, 2, 3, 6).GetParent() == copc.VoxelKey(2, 1, 1, 3)

    # TODO[Leo]: Check if there is a way to avoid self in function definition
    assert not copc.VoxelKey(3, 2, 3, 6).GetParent() == copc.VoxelKey.BaseKey()

    assert copc.VoxelKey(1, 1, 1, 1).GetParent() == copc.VoxelKey.BaseKey()
    assert copc.VoxelKey(1, 1, 1, -1).GetParent() == copc.VoxelKey.InvalidKey()

    assert copc.VoxelKey(1, 1, 1, -1).GetParent().IsValid() is False
    assert copc.VoxelKey(0, 0, 0, 0).GetParent().IsValid() is False


def test_is_child():
    assert (
        copc.VoxelKey(-1, -1, -1, -1).ChildOf(parent_key=copc.VoxelKey.BaseKey())
        is False
    )
    assert copc.VoxelKey.BaseKey().ChildOf(copc.VoxelKey.InvalidKey()) is False

    assert copc.VoxelKey(4, 4, 6, 12).ChildOf(copc.VoxelKey(3, 2, 3, 6))
    assert copc.VoxelKey(3, 2, 3, 6).ChildOf(copc.VoxelKey(2, 1, 1, 3))
    assert copc.VoxelKey(3, 2, 3, 6).ChildOf(copc.VoxelKey.BaseKey())

    assert not copc.VoxelKey(4, 4, 6, 12).ChildOf(copc.VoxelKey(3, 4, 8, 6))
    assert not copc.VoxelKey(3, 2, 3, 6).ChildOf(copc.VoxelKey(2, 2, 2, 2))


def test_get_parents():
    assert len(copc.VoxelKey(-1, -1, -1, -1).GetParents(True)) == 0
    assert len(copc.VoxelKey(-1, -1, -1, -1).GetParents(False)) == 0

    test_keys = [
        copc.VoxelKey(4, 4, 6, 12),
        copc.VoxelKey(3, 2, 3, 6),
        copc.VoxelKey(2, 1, 1, 3),
        copc.VoxelKey(1, 0, 0, 1),
        copc.VoxelKey(0, 0, 0, 0),
    ]
    #
    get_parents_inclusive = test_keys[0].GetParents(include_current=True)
    assert len(get_parents_inclusive) == len(test_keys)
    assert get_parents_inclusive == test_keys

    test_keys_exclusive = test_keys[1:]

    get_parents_non_inclusive = test_keys[0].GetParents(False)
    assert len(get_parents_non_inclusive) == len(test_keys) - 1
    assert get_parents_non_inclusive == test_keys_exclusive


def test_box_constructors():

    # 3D box constructor

    box = copc.Box(0.0, 1.0, 0.0, 1.0, 0.0, 1.0)
    assert box.x_min == 0.0
    assert box.x_max == 1.0
    assert box.y_min == 0.0
    assert box.y_max == 1.0
    assert box.z_min == 0.0
    assert box.z_max == 1.0

    # 2D box constructor
    box = copc.Box(0.0, 1.0, 0.0, 1.0)
    assert box.x_min == 0.0
    assert box.x_max == 1.0
    assert box.y_min == 0.0
    assert box.y_max == 1.0
    assert box.z_min == -float_info.max
    assert box.z_max == float_info.max

    # copc.VoxelKey constructor
    header = copc.LasHeader()
    header.span = 10
    box = copc.Box(copc.VoxelKey(1, 1, 0, 0), header)

    assert box.x_min == 5.0
    assert box.x_max == 10.0
    assert box.y_min == 0.0
    assert box.y_max == 5.0
    assert box.z_min == 0.0
    assert box.z_max == 5.0


def test_box_functions():

    # Intersects
    ## X axis only crosses boundary
    assert copc.Box(1, 2, 0.5, 1).Intersects(copc.Box(0.5, 1.5, 0, 2))
    assert copc.Box(0.5, 1.5, 0, 2).Intersects(copc.Box(1, 2, 0.5, 1))

    assert not copc.Box(1, 2, 0.5, 1).Intersects(copc.Box(2.01, 2.5, 0, 2))
    assert not copc.Box(2.01, 2.5, 0, 2).Intersects(copc.Box(1, 2, 0.5, 1))

    ## If one box contains the other they also intersect
    assert copc.Box(1, 2, 1, 2).Intersects(copc.Box(0, 4, 0, 4))
    assert copc.Box(0, 4, 0, 4).Intersects(copc.Box(1, 2, 1, 2))

    # Contains box
    header = copc.LasHeader()
    header.span = 10
    box1 = copc.Box(copc.VoxelKey(0, 0, 0, 0), header)
    box2 = copc.Box(copc.VoxelKey(1, 1, 0, 0), header)
    assert box1.Contains(box2)
    assert not box2.Contains(box1)
    ## A box contains itself
    assert box2.Contains(box2)

    # Contains vector
    ## 2D box
    assert copc.Box(0, 1, 0, 1).Contains((0.5, 0.5, 5))
    assert copc.Box(0, 1, 0, 1).Contains((0.5, 1, 5))
    assert not copc.Box(0, 1, 0, 1).Contains((0.5, 5, 5))

    ## 3D box
    assert copc.Box(0, 1, 0, 1, 0, 1).Contains((0.5, 0.5, 0.5))
    assert copc.Box(0, 1, 0, 1, 0, 1).Contains((1, 1, 1))
    assert not copc.Box(0, 1, 0, 1, 0, 1).Contains((0.5, 0.5, 5))

    # Within
    header = copc.LasHeader()
    header.span = 10
    box1 = copc.Box(copc.VoxelKey(0, 0, 0, 0), header)
    box2 = copc.Box(copc.VoxelKey(1, 1, 0, 0), header)
    assert not box1.Within(box2)
    assert box2.Within(box1)
    ## A box is within itself
    assert box2.Within(box2)


def test_key_spatial_functions():

    header = copc.LasHeader()
    header.span = 2

    # Intersects
    assert copc.VoxelKey(1, 0, 0, 0).Intersects(copc.Box(0.5, 2, 0.5, 0.75), header)
    assert not copc.VoxelKey(1, 0, 0, 0).Intersects(copc.Box(0.5, 2, 1.5, 2), header)
    assert not copc.VoxelKey(1, 0, 0, 0).Intersects(
        copc.Box(0.5, 1.5, 0.5, 1.5, 1.5, 2), header
    )
    ## If one box contains the other they also intersect
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(copc.Box(0, 2, 0, 2), header)

    # Contains box
    assert copc.VoxelKey(0, 0, 0, 0).Contains(copc.Box(0, 1, 0, 1, 0, 1), header)
    assert not copc.VoxelKey(2, 0, 0, 0).Contains(copc.Box(0, 1, 0, 1, 0, 1), header)
    ## A box contains itself
    assert copc.VoxelKey(0, 0, 0, 0).Contains(
        copc.Box(0, header.span, 0, header.span, 0, header.span), header
    )

    # Contains vector
    assert copc.VoxelKey(0, 0, 0, 0).Contains(copc.Vector3(1, 1, 1), header)
    assert not copc.VoxelKey(0, 0, 0, 0).Contains(copc.Vector3(2.1, 1, 1), header)

    # Within
    assert copc.VoxelKey(1, 1, 1, 1).Within(
        copc.Box(0.99, 2.01, 0.99, 2.01, 0.99, 2.01), header
    )
    ## A box is within itself
    assert copc.VoxelKey(0, 0, 0, 0).Within(
        copc.Box(0, header.span, 0, header.span, 0, header.span), header
    )
