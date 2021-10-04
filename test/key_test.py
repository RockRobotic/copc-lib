from sys import float_info

import copclib as copc
import pytest


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

    assert copc.VoxelKey(3, 2, 3, 6).GetParent() != copc.VoxelKey.BaseKey()

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


def test_key_spatial_functions():

    # Make a LasHeader with span 2
    header = copc.LasHeader()
    header.min = (0, 0, 0)
    header.max = (2, 2, 2)

    # Intersects
    ## Contains
    assert copc.VoxelKey(1, 1, 1, 1).Intersects((1.1, 1.1, 1.1, 1.9, 1.9, 1.9), header)
    ## Crosses
    assert copc.VoxelKey(1, 1, 1, 1).Intersects((1.5, 1.5, 1.5, 2.5, 2.5, 2.5), header)
    assert copc.VoxelKey(1, 1, 1, 1).Intersects((1.5, 1.5, 0.5, 2.5, 2.5, 2.5), header)
    assert copc.VoxelKey(1, 1, 1, 1).Intersects((1.5, 0.5, 1.5, 2.5, 2.5, 2.5), header)
    assert copc.VoxelKey(1, 1, 1, 1).Intersects((0.5, 1.5, 1.5, 2.5, 2.5, 2.5), header)
    ## Equals
    assert copc.VoxelKey(1, 0, 0, 0).Intersects((0, 0, 0, 1, 1, 1), header)
    ## Touches
    assert copc.VoxelKey(1, 0, 0, 0).Intersects((1, 1, 1, 2, 2, 2), header)
    ## Within
    assert copc.VoxelKey(1, 1, 1, 1).Intersects((0, 0, 0, 4, 4, 4), header)
    ## Outside
    assert not copc.VoxelKey(1, 0, 0, 0).Intersects((1, 1, 1.1, 2, 2, 2), header)

    # Contains box
    assert copc.VoxelKey(0, 0, 0, 0).Contains((0, 0, 0, 1, 1, 1), header)
    assert not copc.VoxelKey(2, 0, 0, 0).Contains((0, 0, 0, 1, 1, 1), header)
    ## A box contains itself
    assert copc.VoxelKey(0, 0, 0, 0).Contains(
        (0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan()), header
    )

    # Contains vector
    assert copc.VoxelKey(0, 0, 0, 0).Contains((1, 1, 1), header)
    assert not copc.VoxelKey(0, 0, 0, 0).Contains((2.1, 1, 1), header)

    # Within
    assert copc.VoxelKey(1, 1, 1, 1).Within(
        (0.99, 0.99, 0.99, 2.01, 2.01, 2.01), header
    )
    ## A box is within itself
    assert copc.VoxelKey(0, 0, 0, 0).Within(
        (0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan()), header
    )
