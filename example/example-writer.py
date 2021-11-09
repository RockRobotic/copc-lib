import random
import copclib as copc
import math

random.seed(0)

# In this example, we'll filter the autzen dataset to only contain depth levels 0-3.
def TrimFileExample(compressor_example_flag):

    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")

    # Copy the header to the new file
    cfg = reader.copc_config

    # Now, we can create our actual writer:
    writer = copc.FileWriter("autzen-trimmed.copc.laz", cfg)

    # The root page is automatically created and added for us
    root_page = writer.GetPage()

    # GetAllNodes will load the entire hierarchy under a given key
    for node in reader.GetAllNodes():
        # In this example, we'll only save up to depth level 3.
        if node.key.d > 3:
            continue

        # It's much faster to write and read compressed data, to avoid compression and decompression
        if not compressor_example_flag:
            writer.AddNodeCompressed(
                root_page,
                node.key,
                reader.GetPointDataCompressed(node),
                node.point_count,
            )

        # Alternatively, if we have uncompressed data and want to compress it without writing it to the file,
        # (for example, compress multiple nodes in parallel and have one thread writing the data),
        # we can use the Compressor class:
        else:
            uncompressed_points = reader.GetPointData(node)
            compressed_points = copc.CompressBytes(
                uncompressed_points, writer.copc_config.las_header
            )
            writer.AddNodeCompressed(
                root_page, node.key, compressed_points, node.point_count
            )
    # Make sure we call close to finish writing the file!
    writer.Close()

    # Now, let's test our new file
    new_reader = copc.FileReader("autzen-trimmed.copc.laz")

    # Let's go through each node we've written and make sure it matches the original
    for node in new_reader.GetAllNodes():
        assert new_reader.GetPointDataCompressed(node) == reader.GetPointDataCompressed(
            node.key
        )

        # Similarly, we could retrieve the compressed node data from the file
        # and decompress it later using the Decompressor class
        if compressor_example_flag:
            compressed_points = reader.GetPointDataCompressed(node.key)
            uncompressed_points = copc.DecompressBytes(
                compressed_points, writer.copc_config.las_header, node.point_count
            )
    reader.Close()
    new_reader.Close()


# In this example, we'll filter the points in the autzen dataset based on bounds.
def BoundsTrimFileExample():
    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")
    old_header = reader.copc_config.las_header

    middle = (old_header.max + old_header.min) / 2
    box = copc.Box(middle.x - 200, middle.y - 200, middle.x + 200, middle.y + 200)

    # Copy the header to the new file
    cfg = reader.copc_config

    # Now, we can create our actual writer:
    writer = copc.FileWriter("autzen-bounds-trimmed.copc.laz", cfg)

    # The root page is automatically created and added for us
    root_page = writer.GetPage()

    for node in reader.GetAllNodes():
        if node.key.Within(old_header, box):
            # If node is within the box then add all points (without decompressing)
            writer.AddNodeCompressed(
                root_page,
                node.key,
                reader.GetPointDataCompressed(node),
                node.point_count,
            )
        elif node.key.Intersects(old_header, box):
            # If node only crosses the box then decompress points data and get subset of points that are within the box
            points = reader.GetPoints(node).GetWithin(box)
            writer.AddNode(root_page, node.key, copc.Points(points).Pack())

    # Make sure we call close to finish writing the file!
    writer.Close()

    # Now, let's test our new file
    new_reader = copc.FileReader("autzen-bounds-trimmed.copc.laz")

    # Let's go through each point and make sure they fit in the within the Box
    for node in new_reader.GetAllNodes():
        points = new_reader.GetPoints(node)
        assert points.Within(box)


# In this example, we'll filter the points in the autzen dataset based on resolution.
def ResolutionTrimFileExample():
    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")
    old_header = reader.copc_config.las_header

    resolution = 10
    target_depth = reader.GetDepthAtResolution(resolution)
    # Check that the resolution of the target depth is equal or smaller to the requested resolution
    assert (
        copc.VoxelKey.GetResolutionAtDepth(
            target_depth, old_header, reader.copc_config.copc_info
        )
        <= resolution
    )

    # Copy the header to the new file
    cfg = reader.copc_config

    # Now, we can create our actual writer:
    writer = copc.FileWriter("autzen-resolution-trimmed.copc.laz", cfg)

    # The root page is automatically created and added for us
    root_page = writer.GetPage()

    for node in reader.GetAllNodes():
        if node.key.d <= target_depth:
            writer.AddNodeCompressed(
                root_page,
                node.key,
                reader.GetPointDataCompressed(node),
                node.point_count,
            )

    # Make sure we call close to finish writing the file!
    writer.Close()

    # Now, let's test our new file
    new_reader = copc.FileReader("autzen-resolution-trimmed.copc.laz")

    new_header = new_reader.copc_config.las_header
    new_copc_info = new_reader.copc_config.copc_info

    # Let's go through each node we've written and make sure the resolution is correct
    for node in new_reader.GetAllNodes():
        assert node.key.d <= target_depth

    # Let's make sure the max resolution is at least as much as we requested
    max_octree_depth = new_reader.GetDepthAtResolution(0)
    assert (
        copc.VoxelKey.GetResolutionAtDepth(max_octree_depth, new_header, new_copc_info)
        <= resolution
    )

    # constants


MIN_BOUNDS = copc.Vector3(-2000, -5000, 20)  # Argument Constructor
MAX_BOUNDS = copc.Vector3([5000, 1034, 125])  # List Constructor
NUM_POINTS = 3000


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

    points = copc.Points(
        las_header.point_format_id,
        las_header.scale,
        las_header.offset,
    )
    for i in range(number_points):
        # Create a point with a given point format
        point = points.CreatePoint()
        # point has getters/setters for all attributes
        point.X = random.randint(
            math.ceil(las_header.ApplyInverseScaleX(max(las_header.min.x, minx))),
            math.floor(
                las_header.ApplyInverseScaleX(min(las_header.max.x, minx + step))
            ),
        )
        point.Y = random.randint(
            math.ceil(las_header.ApplyInverseScaleY(max(las_header.min.y, miny))),
            math.floor(
                las_header.ApplyInverseScaleY(min(las_header.max.y, miny + step))
            ),
        )
        point.Z = random.randint(
            math.ceil(las_header.ApplyInverseScaleZ(max(las_header.min.z, minz))),
            math.floor(
                las_header.ApplyInverseScaleZ(min(las_header.max.z, minz + step))
            ),
        )

        # For visualization purposes
        point.point_source_id = key.d + key.x + key.y + key.z

        points.AddPoint(point)
    return points


# In this example, we'll create our own file from scratch
def NewFileExample():

    # Create our new file with the specified format, scale, and offset
    cfg = copc.CopcConfigWriter(
        8, [0.1, 0.1, 0.1], [50, 50, 50], "TEST_WKT", has_extended_stats=True
    )
    # As of now, the library will not automatically compute the min/max of added points
    # so we will have to calculate it ourselves
    cfg.las_header.min = MIN_BOUNDS
    cfg.las_header.max = MAX_BOUNDS

    cfg.copc_info.spacing = 10

    # Now, we can create our COPC writer:
    writer = copc.FileWriter("new-copc.copc.laz", cfg)
    header = writer.copc_config.las_header

    # Set the COPC Extents
    extents = writer.copc_config.copc_extents

    extents.intensity.minimum = 0
    extents.intensity.maximum = 10000
    extents.intensity.mean = 50
    extents.intensity.var = 5

    extents.classification.minimum = 5
    extents.classification.maximum = 201

    # The root page is automatically created
    root_page = writer.GetPage()

    # First we'll add a root node
    key = copc.VoxelKey(0, 0, 0, 0)
    points = RandomPoints(key, header, NUM_POINTS)
    # The node will be written to the file when we call AddNode
    writer.AddNode(root_page, key, points)

    # We can also add pages in the same way, as long as the Key we specify
    # is a child of the parent page

    page = writer.AddSubPage(root_page, (1, 1, 1, 0))

    # Once our page is created, we can add nodes to it like before
    key = copc.VoxelKey(1, 1, 1, 0)
    points = RandomPoints(key, header, NUM_POINTS)
    writer.AddNode(page, key, points)

    key = copc.VoxelKey(2, 2, 2, 0)
    points = RandomPoints(key, header, NUM_POINTS)
    writer.AddNode(page, key, points)

    # We can nest subpages as much as we want, as long as they are children of the parent
    sub_page = writer.AddSubPage(page, (3, 4, 4, 0))
    points = RandomPoints(sub_page.key, header, NUM_POINTS)
    writer.AddNode(page, sub_page.key, points)

    # Make sure we call close to finish writing the file!
    writer.Close()

    # We can check that the spatial bounds of the file have been respected
    reader = copc.FileReader("new-copc.copc.laz")
    assert reader.ValidateSpatialBounds()


if __name__ == "__main__":
    TrimFileExample(False)
    TrimFileExample(True)
    BoundsTrimFileExample()
    ResolutionTrimFileExample()
    NewFileExample()
