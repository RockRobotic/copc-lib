from sys import float_info

import copclib as copc
import pytest


def test_box_constructors():

    # 2D box constructor
    box = copc.Box(1.0, 0.0, 1.0, 1.0)
    assert box.x_min == 1.0
    assert box.y_min == 0.0
    assert box.z_min == -float_info.max
    assert box.x_max == 1.0
    assert box.y_max == 1.0
    assert box.z_max == float_info.max

    with pytest.raises(RuntimeError):
        assert copc.Box(2, 0, 1, 1)
        assert copc.Box(0, 2, 1, 1)

    # 3D box constructor
    box = copc.Box(1.0, 0.0, 0.0, 1.0, 1.0, 1.0)
    assert box.x_min == 1.0
    assert box.y_min == 0.0
    assert box.z_min == 0.0
    assert box.x_max == 1.0
    assert box.y_max == 1.0
    assert box.z_max == 1.0

    with pytest.raises(RuntimeError):
        assert copc.Box(2, 0, 0, 1, 1, 1)
        assert copc.Box(0, 2, 0, 1, 1, 1)
        assert copc.Box(0, 0, 2, 1, 1, 1)

    # Vector3 constructor
    box = copc.Box((1.0, 2.0, 3.0), (4.0, 5.0, 6.0))
    assert box.x_min == 1.0
    assert box.y_min == 2.0
    assert box.z_min == 3.0
    assert box.x_max == 4.0
    assert box.y_max == 5.0
    assert box.z_max == 6.0

    with pytest.raises(RuntimeError):
        assert copc.Box((2, 0, 0), (1, 1, 1))
        assert copc.Box((0, 2, 0), (1, 1, 1))
        assert copc.Box((0, 0, 2), (1, 1, 1))

    # copc.VoxelKey constructor
    # Make a LasHeader with span 10
    header = copc.LasHeader()
    header.min = (0, 0, 0)
    header.max = (10, 10, 10)
    box = copc.Box((1, 1, 0, 0), header)

    assert box.x_min == 5.0
    assert box.y_min == 0.0
    assert box.z_min == 0.0
    assert box.x_max == 10.0
    assert box.y_max == 5.0
    assert box.z_max == 5.0


def test_box_functions():

    # Intersects 2D box
    ## Contains
    assert copc.Box(0, 0, 4, 4).Intersects((1, 1, 2, 2))
    ## Crosses
    assert copc.Box(1, 1, 2, 2).Intersects((1.5, 1.5, 2.5, 2.5))
    assert copc.Box(1, 1, 2, 2).Intersects((1.5, 0.5, 2.5, 2.5))
    assert copc.Box(1, 1, 2, 2).Intersects((0.5, 1.5, 2.5, 2.5))
    ## Equals
    assert copc.Box(0, 0, 1, 1).Intersects((0, 0, 1, 1))
    ## Touches
    assert copc.Box(0, 0, 1, 1).Intersects((1, 1, 2, 2))
    ## Within
    assert copc.Box(1, 1, 2, 2).Intersects((0, 0, 4, 4))
    ## Outside
    assert not copc.Box(0, 0, 1, 1).Intersects((1, 1.1, 2, 2))

    # Intersects 3D box
    ## Contains
    assert copc.Box(0, 0, 0, 4, 4, 4).Intersects((1, 1, 1, 2, 2, 2))
    ## Crosses
    assert copc.Box(1, 1, 1, 2, 2, 2).Intersects((1.5, 1.5, 1.5, 2.5, 2.5, 2.5))
    assert copc.Box(1, 1, 1, 2, 2, 2).Intersects((1.5, 1.5, 0.5, 2.5, 2.5, 2.5))
    assert copc.Box(1, 1, 1, 2, 2, 2).Intersects((1.5, 0.5, 1.5, 2.5, 2.5, 2.5))
    assert copc.Box(1, 1, 1, 2, 2, 2).Intersects((0.5, 1.5, 1.5, 2.5, 2.5, 2.5))
    ## Equals
    assert copc.Box(0, 0, 0, 1, 1, 1).Intersects((0, 0, 0, 1, 1, 1))
    ## Touches
    assert copc.Box(0, 0, 0, 1, 1, 1).Intersects((1, 1, 1, 2, 2, 2))
    ## Within
    assert copc.Box(1, 1, 1, 2, 2, 2).Intersects((0, 0, 0, 4, 4, 4))
    ## Outside
    assert not copc.Box(0, 0, 0, 1, 1, 1).Intersects((1, 1, 1.1, 2, 2, 2))

    # Contains box
    # Make a LasHeader with span 10
    header = copc.LasHeader()
    header.min = (0, 0, 0)
    header.max = (10, 10, 10)
    box1 = copc.Box((0, 0, 0, 0), header)
    box2 = copc.Box((1, 1, 0, 0), header)
    assert box1.Contains(box2)
    assert not box2.Contains(box1)
    ## A box contains itself
    assert box2.Contains(box2)

    # Contains vector
    ## 2D box
    assert copc.Box(0, 0, 1, 1).Contains((0.5, 0.5, 5))
    assert copc.Box(0, 0, 1, 1).Contains((0.5, 1, 5))
    assert not copc.Box(0, 0, 1, 1).Contains((0.5, 5, 5))

    ## 3D box
    assert copc.Box(0, 0, 0, 1, 1, 1).Contains((0.5, 0.5, 0.5))
    assert copc.Box(0, 0, 0, 1, 1, 1).Contains((1, 1, 1))
    assert not copc.Box(0, 0, 0, 1, 1, 1).Contains((0.5, 0.5, 5))

    # Within
    # Make a LasHeader with span 10
    header = copc.LasHeader()
    header.min = (0, 0, 0)
    header.max = (10, 10, 10)
    box1 = copc.Box((0, 0, 0, 0), header)
    box2 = copc.Box((1, 1, 0, 0), header)
    assert not box1.Within(box2)
    assert box2.Within(box1)
    ## A box is within itself
    assert box2.Within(box2)
