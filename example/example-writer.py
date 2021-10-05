import random

import copclib as copc


# In this example, we'll filter the autzen dataset to only contain depth levels 0-3.
def TrimFileExample(compressor_example_flag):

    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")
    old_header = reader.GetLasHeader()

    # Copy the header to the new file
    cfg = copc.LasConfig(old_header, reader.GetExtraByteVlr())

    # Now, we can create our actual writer, with an optional `span` and `wkt`:
    writer = copc.FileWriter(
        "autzen-trimmed.copc.laz",
        cfg,
        reader.GetCopcHeader().span,
        reader.GetWkt(),
    )

    # The root page is automatically created and added for us
    root_page = writer.GetRootPage()

    # GetAllChildren will load the entire hierarchy under a given key
    for node in reader.GetAllChildren(root_page.key):
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
            header = writer.GetLasHeader()
            uncompressed_points = reader.GetPointData(node)
            compressed_points = copc.CompressBytes(
                uncompressed_points, header.point_format_id, cfg.NumExtraBytes()
            )
            writer.AddNodeCompressed(
                root_page, node.key, compressed_points, node.point_count
            )
    # Make sure we call close to finish writing the file!
    writer.Close()

    # Now, let's test our new file
    new_reader = copc.FileReader("autzen-trimmed.copc.laz")

    # Let's go through each node we've written and make sure it matches the original
    for node in new_reader.GetAllChildren():
        assert new_reader.GetPointDataCompressed(node) == reader.GetPointDataCompressed(
            node.key
        )

        # Similarly, we could retrieve the compressed node data from the file
        # and decompress it later using the Decompressor class
        if compressor_example_flag:
            header = writer.GetLasHeader()
            compressed_points = reader.GetPointDataCompressed(node.key)
            uncompressed_points = copc.DecompressBytes(
                compressed_points, header, node.point_count
            )
    reader.Close()
    new_reader.Close()


# In this example, we'll filter the points in the autzen dataset based on bounds.
def BoundsTrimFileExample():
    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")
    old_header = reader.GetLasHeader()

    # Take horizontal 2D box of [200,200] roughly in the middle of the point cloud.
    box = copc.Box(637190, 851109, 637390, 851309)

    # Copy the header to the new file
    cfg = copc.LasConfig(old_header, reader.GetExtraByteVlr())

    # Now, we can create our actual writer, with an optional `span` and `wkt`:
    writer = copc.FileWriter(
        "autzen-bounds-trimmed.copc.laz",
        cfg,
        reader.GetCopcHeader().span,
        reader.GetWkt(),
    )

    # The root page is automatically created and added for us
    root_page = writer.GetRootPage()

    for node in reader.GetAllChildren():
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
    for node in new_reader.GetAllChildren():
        points = new_reader.GetPoints(node)
        assert points.Within(box)


# In this example, we'll filter the points in the autzen dataset based on resolution.
def ResolutionTrimFileExample():
    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")
    old_header = reader.GetLasHeader()

    resolution = 10

    # Copy the header to the new file
    cfg = copc.LasConfig(old_header, reader.GetExtraByteVlr())

    # Now, we can create our actual writer, with an optional `span` and `wkt`:
    writer = copc.FileWriter(
        "autzen-resolution-trimmed.copc.laz",
        cfg,
        reader.GetCopcHeader().span,
        reader.GetWkt(),
    )

    # The root page is automatically created and added for us
    root_page = writer.GetRootPage()
    max_depth = reader.GetDepthAtResolution(resolution)

    for node in reader.GetAllChildren():
        if node.key.d <= max_depth:
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

    new_las_header = new_reader.GetLasHeader()
    new_copc_header = new_reader.GetCopcHeader()
    # Let's make sure the max resolution is at least as much as we requested
    max_depth = new_reader.GetDepthAtResolution(0)
    assert (
        copc.VoxelKey.GetDepthResolution(max_depth, new_las_header, new_copc_header)
        <= resolution
    )


# constants
MIN_BOUNDS = copc.Vector3(-2000, -5000, 20)  # Argument Constructor
MAX_BOUNDS = copc.Vector3([5000, 1034, 125])  # List Constructor
NUM_POINTS = 3000


# This function will generate `NUM_POINTS` random points within the voxel bounds
def RandomPoints(key, point_format_id):

    # Voxel cube dimensions will be calculated from the maximum span of the file
    span = max(
        {
            MAX_BOUNDS.x - MIN_BOUNDS.x,
            MAX_BOUNDS.y - MIN_BOUNDS.y,
            MAX_BOUNDS.z - MIN_BOUNDS.z,
        }
    )
    # Step size accounts for depth level
    step = span / pow(2, key.d)

    minx = MIN_BOUNDS.x + (step * key.x)
    miny = MIN_BOUNDS.y + (step * key.y)
    minz = MIN_BOUNDS.z + (step * key.z)

    random.seed(0)

    points = copc.Points(
        point_format_id,
        copc.Vector3.DefaultScale(),
        copc.Vector3.DefaultOffset(),
    )
    for i in range(NUM_POINTS):
        # Create a point with a given point format
        point = points.CreatePoint()
        # point has getters/setters for all attributes
        point.UnscaledX = int(
            random.uniform(min(minx, MAX_BOUNDS.x), min(minx + step, MAX_BOUNDS.x))
        )

        point.UnscaledY = int(
            random.uniform(min(miny, MAX_BOUNDS.y), min(miny + step, MAX_BOUNDS.y))
        )

        point.UnscaledZ = int(
            random.uniform(min(minz, MAX_BOUNDS.z), min(minz + step, MAX_BOUNDS.z))
        )

        # For visualization purposes
        point.PointSourceID = key.d + key.x + key.y + key.d

        points.AddPoint(point)
    return points


# In this example, we'll create our own file from scratch
def NewFileExample():

    # Create our new file with the specified format, scale, and offset
    cfg = copc.LasConfig(8, [1, 1, 1], [0, 0, 0])
    # As of now, the library will not automatically compute the min/max of added points
    # so we will have to calculate it ourselves
    cfg.min = [
        (MIN_BOUNDS.x * cfg.scale.x) - cfg.offset.x,
        (MIN_BOUNDS.y * cfg.scale.y) - cfg.offset.y,
        (MIN_BOUNDS.z * cfg.scale.z) - cfg.offset.z,
    ]
    cfg.max = [
        (MAX_BOUNDS.x * cfg.scale.x) - cfg.offset.x,
        (MAX_BOUNDS.y * cfg.scale.y) - cfg.offset.y,
        (MAX_BOUNDS.z * cfg.scale.z) - cfg.offset.z,
    ]

    # Now, we can create our COPC writer, with an optional `span` and `wkt`:
    writer = copc.FileWriter("new-copc.copc.laz", cfg, 256, "TEST_WKT")

    # The root page is automatically created
    root_page = writer.GetRootPage()

    # First we'll add a root node
    key = copc.VoxelKey(0, 0, 0, 0)
    points = RandomPoints(key, cfg.point_format_id)
    # The node will be written to the file when we call AddNode
    writer.AddNode(root_page, key, points)

    # We can also add pages in the same way, as long as the Key we specify
    # is a child of the parent page

    page = writer.AddSubPage(root_page, copc.VoxelKey(1, 1, 1, 0))

    # Once our page is created, we can add nodes to it like before
    key = copc.VoxelKey(1, 1, 1, 0)
    points = RandomPoints(key, cfg.point_format_id)
    writer.AddNode(page, key, points)

    key = copc.VoxelKey(2, 2, 2, 0)
    points = RandomPoints(key, cfg.point_format_id)
    writer.AddNode(page, key, points)

    # We can nest subpages as much as we want, as long as they are children of the parent
    sub_page = writer.AddSubPage(page, copc.VoxelKey(3, 4, 4, 2))
    points = RandomPoints(sub_page.key, cfg.point_format_id)
    writer.AddNode(page, sub_page.key, points)

    # Make sure we call close to finish writing the file!
    writer.Close()


if __name__ == "__main__":
    TrimFileExample(False)
    TrimFileExample(True)
    BoundsTrimFileExample()
    ResolutionTrimFileExample()
    NewFileExample()
