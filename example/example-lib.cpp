#include <cstdint>
#include <cstring>
#include <iostream>

#include <copc-lib/copc/copc.hpp>

using namespace copc;

void PrintPoints(std::vector<char> point_vec, int point_record_length)
{
    auto point_buff = point_vec.data();
    for (int i = 0; i < 5; i++)
    {
        int pointOffset = i * point_record_length;

        int32_t x, y, z;

        size_t point_size = sizeof(x);
        std::memcpy(&x, point_buff + pointOffset, point_size);
        std::memcpy(&y, point_buff + pointOffset + point_size, point_size);
        std::memcpy(&z, point_buff + pointOffset + point_size * 2, point_size);
        std::cout << "Point " << i << ": X=" << x << ", Y=" << y << ", Z=" << z << std::endl;
    }
}

int main()
{
    // Call default constructor with Andrew's test file
    Copc copc("test/data/autzen-classified.copc.laz");

    // We can get the CopcData struct
    auto copcHeader = copc.file->GetCopcHeader();
    std::cout << "CopcData: " << std::endl;
    std::cout << "\tSpan: " << copcHeader.span << std::endl
              << "\tRoot Offset: " << copcHeader.root_hier_offset << std::endl
              << "\tRoot Size: " << copcHeader.root_hier_size << std::endl;

    // Get the Las Header
    auto lasHeader = copc.file->GetLasHeader();
    std::cout << std::endl << "Las Header:" << std::endl;
    std::cout << "\tPoint format: " << (int)lasHeader.point_format_id << std::endl
              << "\tPoint count: " << (int)lasHeader.point_count << std::endl;

    // Get the WKT string
    auto wkt = copc.file->GetWkt();
    std::cout << std::endl << "WKT: " << std::endl << wkt << std::endl;

    std::cout << std::endl;

    {
        // Create a new root key (0-0-0-0)
        hierarchy::VoxelKey key(0, 0, 0, 0);

        // FindNode will automatically load the minimum pages needed
        // to find the key you request
        auto rootEntry = copc.hierarchy->FindNode(key);
        std::cout << rootEntry->ToString() << std::endl;

        // If a key doesn't exist, FindNode will return nullptr
        hierarchy::VoxelKey keyDoesNotExist(5, 4, 20, 8);
        if (copc.hierarchy->FindNode(keyDoesNotExist) == nullptr)
            std::cout << std::endl << "Key " << keyDoesNotExist.ToString() << " does not exist!";
    }

    std::cout << std::endl;

    {
        // Now, we will load the point data from a node in the hierarchy
        hierarchy::VoxelKey loadKey(4, 11, 9, 0);

        auto node = copc.hierarchy->FindNode(loadKey);
        std::vector<char> nodeData = node->GetPointData();

        std::cout << std::endl << "First 5 points: " << std::endl;
        PrintPoints(nodeData, lasHeader.point_record_length);
    }

    {
        // Compare that to loading Point objects:
        hierarchy::VoxelKey loadKey(4, 11, 9, 0);

        auto node = copc.hierarchy->FindNode(loadKey);
        auto nodeData = node->GetPoints();

        std::cout << std::endl << "First 5 points: " << std::endl;
        for (int i = 0; i < 5; i++)
        {
            std::cout << "Point " << i << ": " << std::endl
                      << "{" << std::endl
                      << nodeData[i].ToString() << std::endl
                      << "}" << std::endl
                      << std::endl;
        }
    }

    std::cout << std::endl;

    {
        // We should always check for a node's existance before working with it
        hierarchy::VoxelKey invalidKey(20, 4, -1, 0);

        auto node = copc.hierarchy->FindNode(invalidKey);
        if (node != nullptr)
        {
            std::vector<char> nodeData = node->GetPointData();
        }
        else
            std::cout << "Invalid key " << invalidKey.ToString() << "!" << std::endl;
    }
}