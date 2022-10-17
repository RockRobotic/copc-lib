import random
import copclib as copc
import os

random.seed(0)
DATADIRECTORY = os.path.join(os.path.dirname(__file__), "..", "test", "data")
if not os.path.exists(os.path.join(os.path.join(DATADIRECTORY, "out"))):
    os.makedirs(os.path.join(DATADIRECTORY, "out"))

MIN_BOUNDS = copc.Vector3(-2000, -5000, 20)  # Argument Constructor
MAX_BOUNDS = copc.Vector3([5000, 1034, 125])  # List Constructor
NUM_POINTS = 3000


# This function will generate `NUM_POINTS` random points within the voxel bounds
def RandomPoints(las_header, number_points):

    # Create a Points object based on the LAS header
    points = copc.Points(las_header)

    # Populate the points
    for i in range(number_points):
        # Create a point with a given point format
        point = points.CreatePoint()
        # point has getters/setters for all attributes
        point.x = random.uniform(
            las_header.min.x,
            las_header.max.x,
        )
        point.y = random.uniform(
            las_header.min.y,
            las_header.max.y,
        )
        point.z = random.uniform(
            las_header.min.z,
            las_header.max.z,
        )

        points.AddPoint(point)
    return points


def writer_example():
    # In this example, we'll create our own file from scratch

    # Create our new file with the specified format, scale, and offset
    cfg = copc.LazConfigWriter(
        point_format_id=8,
        scale=(0.1, 0.1, 0.1),
        offset=(50, 50, 50),
        wkt="TEST_WKT",
    )
    # As of now, the library will not automatically compute the min/max of added points
    # so we will have to calculate it ourselves
    cfg.las_header.min = MIN_BOUNDS
    cfg.las_header.max = MAX_BOUNDS

    # Now, we can create our COPC writer:
    writer = copc.LazWriter(os.path.join(DATADIRECTORY, "out", "test-laz.laz"), cfg)

    header = writer.laz_config.las_header

    points = RandomPoints(header, NUM_POINTS)
    writer.WritePoints(points)

    points = RandomPoints(header, NUM_POINTS)
    writer.WritePoints(points)

    writer.Close()

    # validate using the reader
    reader = copc.LazReader(os.path.join(DATADIRECTORY, "out", "test-laz.laz"))
    assert reader.laz_config.las_header.point_count == NUM_POINTS * 2
    assert len(reader.GetPoints()) == NUM_POINTS * 2
    print("DONE")


if __name__ == "__main__":
    writer_example()
