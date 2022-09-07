import os
import random
import copclib as copc
import numpy as np

DATADIRECTORY = os.path.join(os.path.dirname(__file__), "data")


def get_autzen_file():
    return os.path.join(DATADIRECTORY, "autzen-classified.copc.laz")


def get_data_dir():
    return DATADIRECTORY


def generate_test_file():
    file_path = os.path.join(DATADIRECTORY, "new-copc.copc.laz")
    if os.path.exists(file_path):
        return file_path

    MIN_BOUNDS = copc.Vector3(-2000, -5000, 20)  # Argument Constructor
    MAX_BOUNDS = copc.Vector3([5000, 1034, 125])  # List Constructor
    NUM_POINTS = 3000

    mins = []
    maxs = []

    # This function will generate `NUM_POINTS` random points within the voxel bounds
    def RandomPoints(key, las_header, number_points):

        # Voxel cube dimensions will be calculated from the maximum spacing of the file
        span = max(
            {
                las_header.max.x - las_header.min.x,
                las_header.max.y - las_header.min.y,
                las_header.max.z - las_header.min.z,
            }
        )
        # Step size accounts for depth level
        step = span / pow(2, key.d)

        minx = las_header.min.x + (step * key.x)
        miny = las_header.min.y + (step * key.y)
        minz = las_header.min.z + (step * key.z)

        # Create a Points object based on the LAS header
        points = copc.Points(las_header)

        # Populate the points
        for i in range(number_points):
            # Create a point with a given point format
            point = points.CreatePoint()
            # point has getters/setters for all attributes
            point.X = random.randint(
                las_header.RemoveScaleX(max(las_header.min.x, minx)),
                las_header.RemoveScaleX(min(las_header.max.x, minx + step)),
            )
            point.Y = random.randint(
                las_header.RemoveScaleY(max(las_header.min.y, miny)),
                las_header.RemoveScaleY(min(las_header.max.y, miny + step)),
            )
            point.Z = random.randint(
                las_header.RemoveScaleZ(max(las_header.min.z, minz)),
                las_header.RemoveScaleZ(min(las_header.max.z, minz + step)),
            )
            point.classification = random.randint(0, 10)

            # For visualization purposes
            point.point_source_id = key.d + key.x + key.y + key.z

            points.AddPoint(point)
        
        mins.append([min(points.x), min(points.y), min(points.z)])
        maxs.append([max(points.x), max(points.y), max(points.z)])
        return points

    cfg = copc.CopcConfigWriter(
        8, [0.01, 0.01, 0.01], [50, 50, 50], "TEST_WKT", has_extended_stats=True
    )
    cfg.las_header.min = MIN_BOUNDS
    cfg.las_header.max = MAX_BOUNDS

    cfg.copc_info.spacing = 10

    # Now, we can create our COPC writer:
    writer = copc.FileWriter(
        file_path, cfg
    )
    header = writer.copc_config.las_header

    key = copc.VoxelKey(0, 0, 0, 0)
    points = RandomPoints(key, header, NUM_POINTS)
    writer.AddNode(key, points)

    page_key = copc.VoxelKey(1, 1, 1, 0)
    key = copc.VoxelKey(1, 1, 1, 0)
    points = RandomPoints(key, header, NUM_POINTS)
    writer.AddNode(key, points, page_key)

    key = copc.VoxelKey(2, 2, 2, 0)
    points = RandomPoints(key, header, NUM_POINTS)
    writer.AddNode(key, points, page_key)

    sub_page_key = copc.VoxelKey(3, 4, 4, 0)
    points = RandomPoints(sub_page_key, header, NUM_POINTS)
    writer.AddNode(sub_page_key, points, sub_page_key)

    writer.copc_config.las_header.min = np.min(mins, axis=0).tolist()
    writer.copc_config.las_header.max = np.max(maxs, axis=0).tolist()

    writer.Close()
    return file_path