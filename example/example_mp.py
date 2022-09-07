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
    """Transformation function that gets called in multiprocessing to rotate a 
    set of copclib.Points
    """

    # Read in the XYZ coordinates
    xyz = np.stack([points.x, points.y, points.z], axis=1)

    # Center about the rotation center
    xyz -= rotation_center
    # Construct the rotation matrix
    rot_mat = R.from_euler("XYZ", (0, 0, 90), degrees=True).as_matrix()
    # Do the rotation
    rotated_points = np.matmul(rot_mat, xyz.T).T
    # Reset the center back to where it was
    rotated_points += rotation_center

    # Assign the rotated points back to the points
    points.x = rotated_points[:, 0]
    points.y = rotated_points[:, 1]
    points.z = rotated_points[:, 2]

    return points


def rotate_file_mp():
    """An example of using transform_multithreaded. It reads in Autzen and rotates all the points 90 degrees,
    updating the header min/max as well.
    """
    # Open a new reader and writer
    file_path = os.path.join(DATADIRECTORY, "out", "autzen-rotated.copc.laz")
    reader = copc.FileReader(os.path.join(DATADIRECTORY, "autzen-classified.copc.laz"))
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    # Set the center of rotation to the minimum XYZ
    min = reader.copc_config.las_header.min
    rotation_center = np.array([min.x, min.y, min.z])
    with tqdm() as progress:
        # Do the transformation, passing the rotation_center as a parameter to _rotate_transform
        transform_multithreaded(
            reader=reader,
            writer=writer,
            transform_function=_rotate_transform,
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
    """An example of the default behavior of transform_multithreaded, 
    which copies the points directly over to a new file.
    """
    # Open the reader and writer
    file_path = os.path.join(DATADIRECTORY, "out", "autzen-rotated.copc.laz")
    reader = copc.FileReader(os.path.join(DATADIRECTORY, "autzen-copied.copc.laz"))
    writer = copc.FileWriter(
        file_path,
        reader.copc_config,
    )

    # Do the transformation
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
