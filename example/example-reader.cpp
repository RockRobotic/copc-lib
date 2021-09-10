#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

#include <copc-lib/io/reader.hpp>
#include <copc-lib/laz/decompressor.hpp>

using namespace copc;
using namespace std;

void PrintPoints(vector<char> point_vec, int point_record_length)
{
    auto point_buff = point_vec.data();
    for (int i = 0; i < 5; i++)
    {
        int point_offset = i * point_record_length;

        int32_t x, y, z;

        size_t point_size = sizeof(x);
        memcpy(&x, point_buff + point_offset, point_size);
        memcpy(&y, point_buff + point_offset + point_size, point_size);
        memcpy(&z, point_buff + point_offset + point_size * 2, point_size);
        cout << "Point " << i << ": X=" << x << ", Y=" << y << ", Z=" << z << endl;
    }
}

int main()
{
    fstream in_stream;
    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);

    // Create a reader object
    Reader reader(&in_stream);

    // We can get the CopcData struct
    auto copc_vlr = reader.GetCopcHeader();
    cout << "CopcData: " << endl;
    cout << "\tSpan: " << copc_vlr.span << endl
         << "\tRoot Offset: " << copc_vlr.root_hier_offset << endl
         << "\tRoot Size: " << copc_vlr.root_hier_size << endl;

    // Get the Las Header
    auto las_header = reader.GetLasHeader();
    cout << endl << "Las Header:" << endl;
    cout << "\tPoint format: " << (int)las_header.point_format_id << endl
         << "\tPoint count: " << (int)las_header.point_count << endl;

    // Get the WKT string
    auto wkt = reader.GetWkt();
    cout << endl << "WKT: " << endl << wkt << endl;

    cout << endl;

    {
        // Create a new root key (0-0-0-0)
        VoxelKey key(0, 0, 0, 0);

        // FindNode will automatically load the minimum pages needed
        // to find the key you request
        Node root_node = reader.FindNode(key);
        cout << root_node.ToString() << endl;

        // If a key doesn't exist, FindNode will return an "invalid" node
        VoxelKey key_does_not_exist(5, 4, 20, 8);
        Node invalid_node = reader.FindNode(key_does_not_exist);
        if (!invalid_node.IsValid())
            cout << endl << "Key " << key_does_not_exist.ToString() << " does not exist!";
    }

    cout << endl;

    {
        // Now, we will load the point data from a node in the hierarchy
        VoxelKey load_key(4, 11, 9, 0);

        Node node = reader.FindNode(load_key);
        vector<char> uncompressed_data = reader.GetPointData(node);

        cout << endl << "First 5 points: " << endl;
        PrintPoints(uncompressed_data, las_header.point_record_length);
    }

    {
        // We can also load into "Point" objects, which have accessors:
        VoxelKey load_key(4, 11, 9, 0);

        Node node = reader.FindNode(load_key);
        auto node_data = reader.GetPoints(node);

        cout << endl << "First 5 points: " << endl;
        for (int i = 0; i < 5; i++)
        {
            cout << "Point " << i << ": " << endl
                 << "{" << endl
                 << node_data[i].ToString() << endl
                 << "}" << endl
                 << endl;
        }
    }

    {
        // Note that trying to get an invalid node's data will result in an exception:
        VoxelKey key_does_not_exist(5, 4, 20, 8);
        Node invalid_node = reader.FindNode(key_does_not_exist);

        try
        {
            auto node_data = reader.GetPointData(invalid_node);
        }
        catch (std::runtime_error)
        {
            cout << "Can't get point data of an invalid node!" << endl;
        }
    }

    {
        // We can also get the raw compressed data if we want to decompress it ourselves:
        VoxelKey loadKey(4, 11, 9, 0);

        Node node = reader.FindNode(loadKey);
        vector<char> compressed_data = reader.GetPointDataCompressed(node);

        // only decompress the first 5 points
        // or we can decompress the entire node by passing node.point_count
        auto uncompressed_data = copc::laz::Decompressor::DecompressBytes(compressed_data, las_header, 5);

        cout << endl << "5 decompressed points: " << endl;
        PrintPoints(uncompressed_data, las_header.point_record_length);
    }
}
