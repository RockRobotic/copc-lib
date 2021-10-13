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


def test_get_children():
    key = copc.VoxelKey.BaseKey()
    children = key.GetChildren()
    for i in range(8):
        assert key.Bisect(i) == children[i]
    assert children[0] == copc.VoxelKey(1, 0, 0, 0)
    assert children[1] == copc.VoxelKey(1, 1, 0, 0)
    assert children[2] == copc.VoxelKey(1, 0, 1, 0)
    assert children[3] == copc.VoxelKey(1, 1, 1, 0)
    assert children[4] == copc.VoxelKey(1, 0, 0, 1)
    assert children[5] == copc.VoxelKey(1, 1, 0, 1)
    assert children[6] == copc.VoxelKey(1, 0, 1, 1)
    assert children[7] == copc.VoxelKey(1, 1, 1, 1)


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


def test_key_spatial_functions():

    # Make a LasHeader with span 2
    header = copc.LasHeader()
    header.min = (0, 0, 0)
    header.max = (2, 2, 2)

    # Intersects
    ## Contains
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(header, (1.1, 1.1, 1.1, 1.9, 1.9, 1.9))
    ## Crosses
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(header, (1.5, 1.5, 1.5, 2.5, 2.5, 2.5))
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(header, (1.5, 1.5, 0.5, 2.5, 2.5, 2.5))
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(header, (1.5, 0.5, 1.5, 2.5, 2.5, 2.5))
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(header, (0.5, 1.5, 1.5, 2.5, 2.5, 2.5))
    ## Equals
    assert copc.VoxelKey(1, 0, 0, 0).Intersects(header, (0, 0, 0, 1, 1, 1))
    ## Touches
    assert copc.VoxelKey(1, 0, 0, 0).Intersects(header, (1, 1, 1, 2, 2, 2))
    ## Within
    assert copc.VoxelKey(1, 1, 1, 1).Intersects(header, (0, 0, 0, 4, 4, 4))
    ## Outside
    assert not copc.VoxelKey(1, 0, 0, 0).Intersects(header, (1, 1, 1.1, 2, 2, 2))

    # Contains box
    assert copc.VoxelKey(0, 0, 0, 0).Contains(header, (0, 0, 0, 1, 1, 1))
    assert not copc.VoxelKey(2, 0, 0, 0).Contains(header, (0, 0, 0, 1, 1, 1))
    ## A box contains itself
    assert copc.VoxelKey(0, 0, 0, 0).Contains(
        header, (0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan())
    )

    # Contains vector
    assert copc.VoxelKey(0, 0, 0, 0).Contains(header, (1, 1, 1))
    assert not copc.VoxelKey(0, 0, 0, 0).Contains(header, (2.1, 1, 1))

    # Within
    assert copc.VoxelKey(1, 1, 1, 1).Within(
        header, (0.99, 0.99, 0.99, 2.01, 2.01, 2.01)
    )
    ## A box is within itself
    assert copc.VoxelKey(0, 0, 0, 0).Within(
        header, (0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan())
    )

    # Crosses
    ## Two touching boxes are also crossing
    assert copc.VoxelKey(1, 0, 0, 0).Crosses(header, (1, 1, 1, 2, 2, 2))
    ## Crossing on all axis
    assert copc.VoxelKey(1, 0, 0, 0).Crosses(header, (0.5, 0.5, 0.5, 1.5, 1.5, 1.5))
    ## Within
    assert not copc.VoxelKey(0, 0, 0, 0).Crosses(
        header, (0, 0, 0, header.GetSpan(), header.GetSpan(), header.GetSpan())
    )
    ## Outside
    assert not copc.VoxelKey(1, 0, 0, 0).Crosses(header, (1.1, 1.1, 1.1, 2, 2, 2))
