import copclib as copc


def reader_example():
    # Create a reader object
    reader = copc.FileReader("autzen-classified.copc.laz")

    # Get the Las Header
    las_header = reader.copc_config.las_header
    cfg = reader.copc_config
    print()
    print("Las Header:")
    print("\tPoint Format: %d" % las_header.point_format_id)
    print("\tPoint Count: %d" % las_header.point_count)

    # Get the Copc Info
    copc_info = reader.copc_config.copc_info
    print("Copc Info: ")
    print("\tSpacing: %d" % copc_info.spacing)
    print("\tRoot Offset: %d" % copc_info.root_hier_offset)
    print("\tRoot Size: %d" % copc_info.root_hier_size)

    # Get the Copc Extents
    copc_extents = reader.copc_config.copc_extents
    print("Copc Extents (Min/Max): ")
    print(
        "\tIntensity: (%f,%f)"
        % (copc_extents.intensity.minimum, copc_extents.intensity.maximum)
    )
    print(
        "\tClassification: (%f,%f)"
        % (copc_extents.classification.minimum, copc_extents.classification.maximum)
    )
    print(
        "\tUser Data: (%f,%f)"
        % (copc_extents.user_data.minimum, copc_extents.user_data.maximum)
    )

    # Get the WKT string
    print("WKT: %s" % reader.copc_config.wkt)

    load_key = (4, 11, 9, 0)

    # FindNode will automatically load the minimum pages needed
    # to find the key you request
    node = reader.FindNode(load_key)
    # If FindNode can't find the node, it will return an "invalid" node:
    if not node.IsValid():
        return
    # GetPoints returns a Points object, which provides helper functions
    # as well as a Get() function to access the underlying point vector
    node_points = reader.GetPoints(node)

    print(node_points)

    print("First 5 points:")
    # Points object supports slicing
    for point in node_points[:5]:
        print(point)

    # We can also get the raw compressed data if we want to decompress it ourselves:

    loadKey = (4, 11, 9, 0)

    node = reader.FindNode(loadKey)
    if not node.IsValid():
        return
    compressed_data = reader.GetPointDataCompressed(node)

    # # We can decompress `n` number of points, or we can decompress the entire node
    # # by setting n=node.point_count
    num_points_to_decompress = 5
    uncompressed_data = copc.DecompressBytes(
        compressed_data, las_header, num_points_to_decompress
    )

    print(
        "Successfully decompressed %d points!"
        % (len(uncompressed_data) / las_header.point_record_length)
    )

    reader.Close()


if __name__ == "__main__":
    reader_example()
