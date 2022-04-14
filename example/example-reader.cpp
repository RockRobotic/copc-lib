#include <copc-lib/io/copc_reader.hpp>
#include <copc-lib/laz/decompressor.hpp>

using namespace copc;
using namespace std;

void ReaderExample()
{
    // Create a reader object
    FileReader reader("autzen-classified.copc.laz");

    // Get the Las Header
    auto las_header = reader.CopcConfig().LasHeader();
    cout << endl << "Las Header:" << endl;
    cout << "\tPoint Format: " << (int)las_header.PointFormatId() << endl
         << "\tPoint Count: " << (int)las_header.PointCount() << endl;

    // Get the CopcInfo struct
    auto copc_info = reader.CopcConfig().CopcInfo();
    cout << "Copc Info: " << endl;
    cout << "\tSpacing: " << copc_info.spacing << endl
         << "\tRoot Offset: " << copc_info.root_hier_offset << endl
         << "\tRoot Size: " << copc_info.root_hier_size << endl;

    // Get the CopcInfo struct
    auto copc_extents = reader.CopcConfig().CopcExtents();
    cout << "Copc Extents (Min/Max): " << endl;
    cout << "\tIntensity : (" << copc_extents.Intensity()->minimum << "/" << copc_extents.Intensity()->maximum << ")"
         << endl
         << "\tClassification : (" << copc_extents.Classification()->minimum << "/"
         << copc_extents.Classification()->maximum << ")" << endl
         << "\tGpsTime : (" << copc_extents.GpsTime()->minimum << "/" << copc_extents.GpsTime()->maximum << ")" << endl;

    // Get the WKT string
    cout << "WKT: " << reader.CopcConfig().Wkt() << endl;

    cout << endl;

    {
        VoxelKey load_key(4, 11, 9, 0);

        // FindNode will automatically load the minimum pages needed
        // to find the key you request
        Node node = reader.FindNode(load_key);
        // If FindNode can't find the node, it will return an "invalid" node:
        if (!node.IsValid())
            exit(123);

        // GetPoints returns a Points object, which provides helper functions
        // as well as a Get() function to access the underlying point vector
        las::Points node_points = reader.GetPoints(node);

        cout << node_points.ToString() << endl;

        cout << endl << "First 5 points: " << endl;
        for (int i = 0; i < 5; i++)
        {
            cout << node_points.Get(i)->ToString() << endl;
        }
    }

    // We can also get the raw compressed data if we want to decompress it ourselves:
    {
        VoxelKey loadKey(4, 11, 9, 0);

        Node node = reader.FindNode(loadKey);
        if (!node.IsValid())
            exit(123);
        vector<char> compressed_data = reader.GetPointDataCompressed(node);

        // We can decompress `n` number of points, or we can decompress the entire node
        // by setting n=node.point_count
        int num_points_to_decompress = 5;
        vector<char> uncompressed_data =
            copc::laz::Decompressor::DecompressBytes(compressed_data, las_header, num_points_to_decompress);

        cout << endl
             << "Successfully decompressed " << uncompressed_data.size() / las_header.PointRecordLength() << "points!"
             << endl;
    }
}

int main() { ReaderExample(); }
