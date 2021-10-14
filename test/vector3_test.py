import pickle
import copclib as copc


def test_vector3():
    # Constructors

    vec = copc.Vector3()
    assert vec.x == 0.0
    assert vec.y == 0.0
    assert vec.z == 0.0

    vec = copc.Vector3(1.0, 2.0, 3.0)
    assert vec.x == 1.0
    assert vec.y == 2.0
    assert vec.z == 3.0

    vec = copc.Vector3((1.0, 2.0, 3.0))
    assert vec.x == 1.0
    assert vec.y == 2.0
    assert vec.z == 3.0

    vec = copc.Vector3.DefaultScale()
    assert vec.x == 0.01
    assert vec.y == 0.01
    assert vec.z == 0.01

    vec = copc.Vector3.DefaultOffset()
    assert vec.x == 0.0
    assert vec.y == 0.0
    assert vec.z == 0.0

    # Vector3 Operators

    vec = copc.Vector3(1.0, 2.0, 3.0)
    other = copc.Vector3(1.0, 2.0, 3.0)

    res = vec * other

    assert res.x == 1.0
    assert res.y == 4.0
    assert res.z == 9.0

    res = vec / other

    assert res.x == 1.0
    assert res.y == 1.0
    assert res.z == 1.0

    res = vec // (2.0, 2.0, 2.0)

    assert res.x == 0.0
    assert res.y == 1.0
    assert res.z == 1.0

    res = vec + other

    assert res.x == 2.0
    assert res.y == 4.0
    assert res.z == 6.0

    res = vec - other

    assert res.x == 0.0
    assert res.y == 0.0
    assert res.z == 0.0

    # Double Operators

    vec = copc.Vector3(1.0, 2.0, 3.0)
    other_vec = vec
    assert other_vec.x == 1.0
    assert other_vec.y == 2.0
    assert other_vec.z == 3.0
    assert other_vec == vec

    other_vec = vec * 2

    assert other_vec.x == 2.0
    assert other_vec.y == 4.0
    assert other_vec.z == 6.0
    assert other_vec != vec

    vec2 = vec / 2

    assert vec2.x == 0.5
    assert vec2.y == 1.0
    assert vec2.z == 1.5
    assert vec2 != vec

    vec2 = vec // 2

    assert vec2.x == 0.0
    assert vec2.y == 1.0
    assert vec2.z == 1.0
    assert vec2 != vec

    vec2 = vec + 2

    assert vec2.x == 3.0
    assert vec2.y == 4.0
    assert vec2.z == 5.0
    assert vec2 != vec

    vec2 = vec - 2

    assert vec2.x == -1.0
    assert vec2.y == 0.0
    assert vec2.z == 1.0
    assert vec2 != vec
