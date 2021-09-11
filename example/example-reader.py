import copclib

# def print_points(point_list, point_record_length):
#     for i in range(5):
#         point_offset = i * point_record_length
#         point_buff = ""
#         point_buff.join(point_list)
#
#         point_size = sys.getsizeof(int)
#
#         x = int.from_bytes(bytearray(point_buff[point_offset: point_offset + point_size]), byteorder='big')
#         y = int.from_bytes(bytearray(point_buff[point_offset + point_size: point_offset + point_size*2]), byteorder='big')
#         z = int.from_bytes(bytearray(point_buff[point_offset + point_size*2: point_offset + point_size*3]), byteorder='big')
#
#         print("Point %d: X=%d, Y=%d, Z=%d" % (i, x, y, z))


if __name__ == "__main__":
    # Create a reader object
    reader = copclib.FileReader("../test/data/autzen-classified.copc.laz")

    # # We can get the CopcData struct
    # copc_vlr = reader.GetCopcHeader()
    # print("CopcData:")
    # print("\tSpan: %d" % copc_vlr.span)
    # print("\tRoot Offset: %d" % copc_vlr.root_hier_offset)
    # print("\tRoot Size: %d" % copc_vlr.root_hier_size)
    #
    # # Get the Las Header
    # las_header = reader.GetLasHeader()
    # print("Las Header:")
    # print("\tPoint Format %d" % las_header.point_format_id)
    # print("\tPoint Count %d" % las_header.point_count)
    #
    # # Get the WKT string
    # wkt = reader.GetWkt()
    # print("\nWKT: %s\n" % wkt)
    #
    # # Create a new root key (0-0-0-0)
    # key = copclib.VoxelKey(0, 0, 0, 0)
#
#     // FindNode will automatically load the minimum pages needed
#     // to find the key you request
#     Node root_node = reader.FindNode(key);
#     cout << root_node.ToString() << endl;
#
#     // If a key doesn't exist, FindNode will return an "invalid" node
#     VoxelKey key_does_not_exist(5, 4, 20, 8);
#     Node invalid_node = reader.FindNode(key_does_not_exist);
#     if (!invalid_node.IsValid())
#         cout << endl << "Key " << key_does_not_exist.ToString() << " does not exist!";
#
# cout << endl;
#
# {
#     // Now, we will load the point data from a node in the hierarchy
#     VoxelKey load_key(4, 11, 9, 0);
#
#     Node node = reader.FindNode(load_key);
#     vector<char> uncompressed_data = reader.GetPointData(node);
#
#     cout << endl << "First 5 points: " << endl;
#     PrintPoints(uncompressed_data, las_header.point_record_length);
# }
#
# {
#     // We can also load into "Point" objects, which have accessors:
#     VoxelKey load_key(4, 11, 9, 0);
#
#     Node node = reader.FindNode(load_key);
#     auto node_data = reader.GetPoints(node);
#
#     cout << endl << "First 5 points: " << endl;
#     for (int i = 0; i < 5; i++)
#     {
#         cout << "Point " << i << ": " << endl
#              << "{" << endl
#              << node_data[i].ToString() << endl
#              << "}" << endl
#              << endl;
#     }
# }
#
# {
#     // Note that trying to get an invalid node's data will result in an exception:
#     VoxelKey key_does_not_exist(5, 4, 20, 8);
#     Node invalid_node = reader.FindNode(key_does_not_exist);
#
#     try
#     {
#         auto node_data = reader.GetPointData(invalid_node);
#     }
#     catch (std::runtime_error)
#     {
#         cout << "Can't get point data of an invalid node!" << endl;
#     }
# }
#
# {
#     // We can also get the raw compressed data if we want to decompress it ourselves:
#     VoxelKey loadKey(4, 11, 9, 0);
#
#     Node node = reader.FindNode(loadKey);
#     vector<char> compressed_data = reader.GetPointDataCompressed(node);
#
#     // only decompress the first 5 points
#     // or we can decompress the entire node by passing node.point_count
#     auto uncompressed_data = copc::laz::Decompressor::DecompressBytes(compressed_data, las_header, 5);
#
#     cout << endl << "5 decompressed points: " << endl;
#     PrintPoints(uncompressed_data, las_header.point_record_length);
# }
