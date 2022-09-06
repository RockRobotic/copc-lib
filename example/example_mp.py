import copclib as copc
from copclib.mp.transform import transform_multithreaded
from scipy.spatial.transform import Rotation as R
import numpy as np

from tqdm import tqdm
import os


DATADIRECTORY = os.path.join(os.path.dirname(__file__), "..", "test", "data")
if not os.path.exists(os.path.join(os.path.join(DATADIRECTORY, "out"))):
    os.makedirs(os.path.join(DATADIRECTORY, "out"))


def _rotate_transform(rotation_center, points, **kwargs):
    xyz = np.stack([points.x, points.y, points.z], axis=1)

    # Rotation matrix
    xyz -= rotation_center
    # print(xyz)
    rot_mat = R.from_euler("XYZ", (0, 0, 90), degrees=True).as_matrix()

    rotated_points = np.matmul(rot_mat, xyz.T).T
    rotated_points += rotation_center

    points.x = rotated_points[:, 0]
    points.y = rotated_points[:, 1]
    points.z = rotated_points[:, 2]

    return points


def rotate_file_mp():
    file_path = os.path.join(DATADIRECTORY, "out", "autzen-rotated.copc.laz")
    reader = copc.FileReader(os.path.join(DATADIRECTORY, "autzen-classified.copc.laz"))
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    min = reader.copc_config.las_header.min
    rotation_center = np.array([min.x, min.y, min.z])
    with tqdm() as progress:
        transform_multithreaded(
            reader,
            writer,
            _rotate_transform,
            transform_function_args=dict(rotation_center=rotation_center),
            progress=progress,
            update_minmax=True,
        )

    writer.Close()

    # validate
    new_reader = copc.FileReader(file_path)
    for node in reader.GetAllNodes():
        assert node.IsValid()
        new_node = new_reader.FindNode(node.key)
        assert new_node.IsValid()
        assert new_node.key == node.key
        assert new_node.point_count == node.point_count


def copy_file_mp():
    file_path = os.path.join(DATADIRECTORY, "out", "autzen-rotated.copc.laz")
    reader = copc.FileReader(os.path.join(DATADIRECTORY, "autzen-copied.copc.laz"))
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    transform_multithreaded(reader, writer)
    writer.Close()

    # validate
    new_reader = copc.FileReader(file_path)
    for node in reader.GetAllNodes():
        assert node.IsValid()
        new_node = new_reader.FindNode(node.key)
        assert new_node.IsValid()
        assert new_node.key == node.key
        assert new_node.point_count == node.point_count
        assert new_node.byte_size == node.byte_size


if __name__ == "__main__":
    rotate_file_mp()
    copy_file_mp()
