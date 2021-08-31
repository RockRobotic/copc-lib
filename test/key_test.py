import copclib


def test_key_validity():
    assert copclib.VoxelKey().IsValid() == False
    assert copclib.VoxelKey(-1, -1, -1, -1).IsValid() == False
    assert copclib.VoxelKey(-1, 0, 0, 0).IsValid() == False
    assert copclib.VoxelKey(0, -1, 0, 0).IsValid() == False
    assert copclib.VoxelKey(0, 0, -1, 0).IsValid() == False
    assert copclib.VoxelKey(0, 0, 0, -1).IsValid() == False
    assert copclib.VoxelKey(0, 0, 0, 0).IsValid() == True
    assert copclib.VoxelKey(1, 1, 1, 1).IsValid() == True


def test_key_operators():
    assert copclib.VoxelKey(0, 0, 0, 0) == copclib.VoxelKey(0, 0, 0, 0)
    assert copclib.VoxelKey(-1, -1, -1, -1) == copclib.VoxelKey(-1, -1, -1, -1)
    assert copclib.VoxelKey(0, 0, 0, 0) != copclib.VoxelKey(1, 1, 1, 1)


def test_get_parent():
    assert copclib.VoxelKey(-1, -1, -1, -1).GetParent().IsValid() == False

    assert copclib.VoxelKey(4, 4, 6, 12).GetParent() == copclib.VoxelKey(3, 2, 3, 6)
    assert copclib.VoxelKey(4, 5, 6, 13).GetParent() == copclib.VoxelKey(3, 2, 3, 6)
    assert copclib.VoxelKey(3, 2, 3, 6).GetParent() == copclib.VoxelKey(2, 1, 1, 3)

    assert copclib.VoxelKey(3, 2, 3, 6).GetParent() != copclib.VoxelKey(0, 0, 0, 0)

    assert copclib.VoxelKey(1, 1, 1, 1).GetParent() == copclib.VoxelKey(0, 0, 0, 0)
    assert copclib.VoxelKey(1, 1, 1, -1).GetParent() == copclib.VoxelKey(-1, -1, -1, -1)

    assert copclib.VoxelKey(1, 1, 1, -1).GetParent().IsValid() == False
    assert copclib.VoxelKey(0, 0, 0, 0).GetParent().IsValid() == False


def test_get_parents():
    assert len(copclib.VoxelKey(-1, -1, -1, -1).GetParents(True)) == 0
    assert len(copclib.VoxelKey(-1, -1, -1, -1).GetParents(False)) == 0

    test_keys = [copclib.VoxelKey(4, 4, 6, 12),
                 copclib.VoxelKey(3, 2, 3, 6),
                 copclib.VoxelKey(2, 1, 1, 3),
                 copclib.VoxelKey(1, 0, 0, 1),
                 copclib.VoxelKey(0, 0, 0, 0)
                 ]
    #
    get_parents_inclusive = test_keys[0].GetParents(True)
    assert len(get_parents_inclusive) == len(test_keys)
    assert get_parents_inclusive == test_keys

    test_keys_exclusive = test_keys[1:]

    get_parents_non_inclusive = test_keys[0].GetParents(False)
    assert len(get_parents_non_inclusive) == len(test_keys) - 1
    assert get_parents_non_inclusive == test_keys_exclusive