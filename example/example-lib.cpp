#include <cstdint>
#include <cstring>
#include <iostream>

#include "copc_lib.hpp"

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
    copc::Copc copc("test/data/autzen-classified.copc.laz");

    // We can get the CopcData struct
    auto copcHeader = copc.GetCopcData();
    std::cout << "CopcData: " << std::endl;
    std::cout << "\tSpan: " << copcHeader.span << std::endl
              << "\tRoot Offset: " << copcHeader.root_hier_offset << std::endl
              << "\tRoot Size: " << copcHeader.root_hier_size << std::endl;

    // Get the Las Header
    auto lasHeader = copc.GetHeader();
    std::cout << std::endl << "Las Header:" << std::endl;
    std::cout << "\tPoint format: " << (int)lasHeader.point_format_id << std::endl
              << "\tPoint count: " << (int)lasHeader.point_count << std::endl;

    // Get the WKT string
    auto wkt = copc.GetWktData();
    std::cout << std::endl << "WKT: " << std::endl << wkt << std::endl;

    // Create a new root key (0-0-0-0)
    copc::VoxelKey key(0, 0, 0, 0);
    // Get the hierarchy node Entry for the root key
    copc::Entry rootEntry = copc.GetEntryFromKey(key);

    // GetEntryFromKey will automatically load the minimum pages needed
    // to find the key you request, so the page that contains 0-0-0-0
    // has already been requested and loaded
    bool isRootSubpage = rootEntry.IsSubpage();

    std::cout << std::endl
              << "Root Node: " << std::endl
              << "\tNode " << rootEntry.ToString() << std::endl
              << "\tKey: " << rootEntry.key.ToString() << std::endl
              << "\tIsSubpage: " << isRootSubpage << std::endl
              << "\tPointCount: " << rootEntry.pointCount << std::endl;

    // We can check if a key exists somewhere in the hierarchy with DoesKeyExist
    copc::VoxelKey keyDoesNotExist(5, 4, 20, 8);
    if (!copc.DoesKeyExist(key))
        std::cout << std::endl << "Key " << keyDoesNotExist.ToString() << " does not exist!";

    // Now, we will load all the point data from a node in the hierarchy
    copc::VoxelKey loadKey(4, 11, 9, 0);

    // Now we can load the node into memory with a voxelkey
    // Note that loadEntry may be invalid, in which case
    // point_vec will be empty.
    {
        copc::Entry loadEntry = copc.GetEntryFromKey(loadKey);
        // We can also check if an entry is valid ourselves by calling
        // loadEntry.IsValid()
        auto point_vec = copc.ReadNodeData(loadEntry);

        if (point_vec.size() == 0)
            std::cout << "Invalid entry " << loadEntry.ToString() << " given to ReadNodeData!" << std::endl;
        else
        {
            std::cout << std::endl << "First 5 points: " << std::endl;
            PrintPoints(point_vec, lasHeader.point_record_length);
        }
    }

    std::cout << std::endl;

    // We can also load a node directly from a VoxelKey object.
    // If the key isn't found, point_vec will again be empty..
    {
        // Now, we can read the node into point_vec
        copc::VoxelKey invalidKey(20, 4, -1, 0);
        auto point_vec = copc.ReadNodeData(invalidKey);

        if (point_vec.size() == 0)
            std::cout << "Invalid key " << invalidKey.ToString() << " given to ReadNodeData!" << std::endl;
        else
        {
            std::cout << std::endl << "First 5 points: " << std::endl;
            PrintPoints(point_vec, lasHeader.point_record_length);
        }
    }
}