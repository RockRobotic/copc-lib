import random
import copclib as copc

random.seed(0)

# In this example, we'll filter the autzen dataset to only contain depth levels 0-3.
def TrimFileExample(compressor_example_flag):

    # We'll get our point data from this file
    reader = copc.FileReader("autzen-classified.copc.laz")

    # Copy the header to the new file
    cfg = reader.GetCopcConfig()

    # Now, we can create our actual writer:
    writer = copc.FileWriter("autzen-trimmed.copc.laz", cfg)

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
            header = writer.las_header
            uncompressed_points = reader.GetPointData(node)
            compressed_points = copc.CompressBytes(
                uncompressed_points, writer.las_header
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
            header = writer.las_header
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
    old_header = reader.las_header

    # Take horizontal 2D box of [200,200] roughly in the middle of the point cloud.
    box = copc.Box(637190, 851109, 637390, 851309)

    # Copy the header to the new file
    cfg = reader.GetCopcConfig()

    # Now, we can create our actual writer:
    writer = copc.FileWriter("autzen-bounds-trimmed.copc.laz", cfg)

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
    old_header = reader.las_header

    resolution = 10
    target_depth = reader.GetDepthAtResolution(resolution)
    # Check that the resolution of the target depth is equal or smaller to the requested resolution
    assert (
        copc.VoxelKey.GetResolutionAtDepth(target_depth, old_header, reader.copc_info)
        <= resolution
    )

    # Copy the header to the new file
    cfg = reader.GetCopcConfig()

    # Now, we can create our actual writer:
    writer = copc.FileWriter("autzen-resolution-trimmed.copc.laz", cfg)

    # The root page is automatically created and added for us
    root_page = writer.GetRootPage()

    for node in reader.GetAllChildren():
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

    new_header = new_reader.las_header
    new_copc_info = new_reader.copc_info

    # Let's go through each node we've written and make sure the resolution is correct
    for node in new_reader.GetAllChildren():
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
        point.UnscaledX = int(
            random.uniform(
                min(minx, las_header.max.x), min(minx + step, las_header.max.x)
            )
        )

        point.UnscaledY = int(
            random.uniform(
                min(miny, las_header.max.y), min(miny + step, las_header.max.y)
            )
        )

        point.UnscaledZ = int(
            random.uniform(
                min(minz, las_header.max.z), min(minz + step, las_header.max.z)
            )
        )

        # For visualization purposes
        point.PointSourceID = key.d + key.x + key.y + key.d

        points.AddPoint(point)
    return points


# In this example, we'll create our own file from scratch
def NewFileExample():

    # Create our new file with the specified format, scale, and offset
    cfg = copc.CopcConfig(8, [1, 1, 1], [0, 0, 0])
    # As of now, the library will not automatically compute the min/max of added points
    # so we will have to calculate it ourselves
    cfg.las_header_base.min = (
        MIN_BOUNDS * cfg.las_header_base.scale - cfg.las_header_base.offset
    )
    cfg.las_header_base.max = (
        MAX_BOUNDS * cfg.las_header_base.scale - cfg.las_header_base.offset
    )
    cfg.copc_info.spacing = 10
    cfg.wkt = "TEST_WKT"

    # Now, we can create our COPC writer:
    writer = copc.FileWriter("new-copc.copc.laz", cfg)

    # The root page is automatically created
    root_page = writer.GetRootPage()
    las_header = writer.las_header

    # First we'll add a root node
    key = copc.VoxelKey(0, 0, 0, 0)
    points = RandomPoints(key, las_header, NUM_POINTS)
    # The node will be written to the file when we call AddNode
    writer.AddNode(root_page, key, points)

    # We can also add pages in the same way, as long as the Key we specify
    # is a child of the parent page

    page = writer.AddSubPage(root_page, (1, 1, 1, 0))

    # Once our page is created, we can add nodes to it like before
    key = copc.VoxelKey(1, 1, 1, 0)
    points = RandomPoints(key, las_header, NUM_POINTS)
    writer.AddNode(page, key, points)

    key = copc.VoxelKey(2, 2, 2, 0)
    points = RandomPoints(key, las_header, NUM_POINTS)
    writer.AddNode(page, key, points)

    # We can nest subpages as much as we want, as long as they are children of the parent
    sub_page = writer.AddSubPage(page, (3, 4, 4, 2))
    points = RandomPoints(sub_page.key, las_header, NUM_POINTS)
    writer.AddNode(page, sub_page.key, points)

    # Make sure we call close to finish writing the file!
    writer.Close()


if __name__ == "__main__":
    TrimFileExample(False)
    TrimFileExample(True)
    BoundsTrimFileExample()
    ResolutionTrimFileExample()
    NewFileExample()
