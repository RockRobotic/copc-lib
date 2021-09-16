import random

import copclib as copc


# In this example, we'll filter the autzen dataset to only contain depth levels 0-3.
def TrimFileExample(compressor_example_flag):

    # We'll get our point data from this file
    reader = copc.FileReader("../test/data/autzen-classified.copc.laz")
    old_header = reader.GetLasHeader()

    # Copy the header to the new file
    cfg = copc.LasConfig(old_header, reader.GetExtraByteVlr())

    # Now, we can create our actual writer, with an optional `span` and `wkt`:
    writer = copc.FileWriter(
        "../test/data/autzen-trimmed.copc.laz",
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
            break

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
                uncompressed_points, header.point_format_id, len(cfg.extra_bytes.items)
            )
            writer.AddNodeCompressed(
                root_page, node.key, compressed_points, node.point_count
            )
    # Make sure we call close to finish writing the file!
    writer.Close()

    # Now, let's test our new file
    new_reader = copc.FileReader("../test/data/autzen-trimmed.copc.laz")

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


# constants
MIN_BOUNDS = copc.Vector3(-2000, -5000, 20)
MAX_BOUNDS = copc.Vector3(5000, 1034, 125)
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

    points = copc.Points(point_format_id)
    for i in range(NUM_POINTS):
        # Create a point with a given point format
        point = copc.Point(point_format_id)
        # point has getters/setters for all attributes
        point.X = int(
            random.uniform(min(minx, MAX_BOUNDS.x), min(minx + step, MAX_BOUNDS.x))
        )

        point.Y = int(
            random.uniform(min(miny, MAX_BOUNDS.y), min(miny + step, MAX_BOUNDS.y))
        )

        point.Z = int(
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
    writer = copc.FileWriter("../test/data/new-copc.copc.laz", cfg, 256, "TEST_WKT")

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
    NewFileExample()
