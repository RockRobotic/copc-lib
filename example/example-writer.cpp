#include <cassert>
#include <cstdint>
#include <random>

#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/header.hpp>
#include <copc-lib/laz/compressor.hpp>
#include <copc-lib/laz/decompressor.hpp>

using namespace copc;
using namespace std;

// In this example, we'll filter the autzen dataset to only contain depth levels 0-3.
void TrimFileExample(bool compressor_example_flag)
{
    // We'll get our point data from this file
    FileReader reader("autzen-classified.copc.laz");
    auto old_header = reader.GetLasHeader();

    {
        // Copy the header to the new file
        Writer::LasConfig cfg(old_header, reader.GetExtraByteVlr());

        // Now, we can create our actual writer, with an optional `span` and `wkt`:
        FileWriter writer("autzen-trimmed.copc.laz", cfg, reader.GetCopcHeader().span, reader.GetWkt());

        // The root page is automatically created and added for us
        Page root_page = writer.GetRootPage();

        // GetAllChildren will load the entire hierarchy under a given key
        for (const auto &node : reader.GetAllChildren(root_page.key))
        {
            // In this example, we'll only save up to depth level 3.
            if (node.key.d > 3)
                continue;

            if (!compressor_example_flag)
            {
                // It's much faster to write and read compressed data, to avoid compression and decompression
                writer.AddNodeCompressed(root_page, node.key, reader.GetPointDataCompressed(node), node.point_count);
            }
            else
            {
                // Alternatively, if we have uncompressed data and want to compress it without writing it to the file,
                // (for example, compress multiple nodes in parallel and have one thread writing the data),
                // we can use the Compressor class:

                std::vector<char> uncompressed_points = reader.GetPointData(node);
                std::vector<char> compressed_points =
                    laz::Compressor::CompressBytes(uncompressed_points, writer.GetLasHeader());
                writer.AddNodeCompressed(root_page, node.key, compressed_points, node.point_count);
            }
        }

        // Make sure we call close to finish writing the file!
        writer.Close();
    }

    // Now, let's test our new file
    FileReader new_reader("autzen-trimmed.copc.laz");

    // Let's go through each node we've written and make sure it matches the original
    for (const auto &node : new_reader.GetAllChildren())
    {
        assert(new_reader.GetPointDataCompressed(node) == reader.GetPointDataCompressed(node.key));

        // Similarly, we could retrieve the compressed node data from the file
        // and decompress it later using the Decompressor class
        if (compressor_example_flag)
        {
            las::LasHeader header = new_reader.GetLasHeader();
            std::vector<char> compressed_points = reader.GetPointDataCompressed(node.key);
            std::vector<char> uncompressed_points =
                laz::Decompressor::DecompressBytes(compressed_points, header, node.point_count);
        }
    }
}

// constants
const Vector3 MIN_BOUNDS = {-2000, -5000, 20};
const Vector3 MAX_BOUNDS = {5000, 1034, 125};
const int NUM_POINTS = 3000;

// random num devices
std::random_device rd;  // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator

// This function will generate `NUM_POINTS` random points within the voxel bounds
las::Points RandomPoints(VoxelKey key, int8_t point_format_id)
{
    // Voxel cube dimensions will be calculated from the maximum span of the file
    double span = std::max({MAX_BOUNDS.x - MIN_BOUNDS.x, MAX_BOUNDS.y - MIN_BOUNDS.y, MAX_BOUNDS.z - MIN_BOUNDS.z});
    // Step size accounts for depth level
    double step = span / std::pow(2, key.d);

    double minx = MIN_BOUNDS.x + (step * key.x);
    double miny = MIN_BOUNDS.y + (step * key.y);
    double minz = MIN_BOUNDS.z + (step * key.z);

    // Random num generators between the min and max spatial bounds of the voxel
    std::uniform_int_distribution<> rand_x((int)std::min(minx, MAX_BOUNDS.x), (int)std::min(minx + step, MAX_BOUNDS.x));
    std::uniform_int_distribution<> rand_y((int)std::min(miny, MAX_BOUNDS.y), (int)std::min(miny + step, MAX_BOUNDS.y));
    std::uniform_int_distribution<> rand_z((int)std::min(minz, MAX_BOUNDS.z), (int)std::min(minz + step, MAX_BOUNDS.z));

    las::Points points(point_format_id, {1, 1, 1}, {0, 0, 0});
    for (int i = 0; i < NUM_POINTS; i++)
    {
        // Create a point with a given point format
        // The use of las::Point constructor is strongly discouraged, instead use las::Points::CreatePoint
        auto point = points.CreatePoint();
        // point has getters/setters for all attributes
        point->UnscaledX(rand_x(gen));
        point->UnscaledY(rand_y(gen));
        point->UnscaledZ(rand_z(gen));
        // For visualization purposes
        point->PointSourceID(key.d + key.x + key.y + key.d);

        points.AddPoint(point);
    }
    return points;
}

// In this example, we'll create our own file from scratch
void NewFileExample()
{
    // Create our new file with the specified format, scale, and offset
    Writer::LasConfig cfg(8, {1, 1, 1}, {0, 0, 0});
    // As of now, the library will not automatically compute the min/max of added points
    // so we will have to calculate it ourselves
    cfg.min = Vector3{(MIN_BOUNDS.x * cfg.scale.x) - cfg.offset.x, (MIN_BOUNDS.y * cfg.scale.y) - cfg.offset.y,
                      (MIN_BOUNDS.z * cfg.scale.z) - cfg.offset.z};
    cfg.max = Vector3{(MAX_BOUNDS.x * cfg.scale.x) - cfg.offset.x, (MAX_BOUNDS.y * cfg.scale.y) - cfg.offset.y,
                      (MAX_BOUNDS.z * cfg.scale.z) - cfg.offset.z};

    // Now, we can create our COPC writer, with an optional `span` and `wkt`:
    FileWriter writer("new-copc.copc.laz", cfg, 256, "TEST_WKT");

    // The root page is automatically created
    Page root_page = writer.GetRootPage();

    // First we'll add a root node
    VoxelKey key(0, 0, 0, 0);
    auto points = RandomPoints(key, cfg.point_format_id);
    // The node will be written to the file when we call AddNode
    writer.AddNode(root_page, key, points);

    // We can also add pages in the same way, as long as the Key we specify
    // is a child of the parent page
    {
        auto page = writer.AddSubPage(root_page, VoxelKey(1, 1, 1, 0));

        // Once our page is created, we can add nodes to it like before
        key = VoxelKey(1, 1, 1, 0);
        points = RandomPoints(key, cfg.point_format_id);
        writer.AddNode(page, key, points);

        key = VoxelKey(2, 2, 2, 0);
        points = RandomPoints(key, cfg.point_format_id);
        writer.AddNode(page, key, points);

        // We can nest subpages as much as we want, as long as they are children of the parent
        auto sub_page = writer.AddSubPage(page, VoxelKey(3, 4, 4, 2));
        points = RandomPoints(sub_page.key, cfg.point_format_id);
        writer.AddNode(page, sub_page.key, points);
    }

    // Make sure we call close to finish writing the file!
    writer.Close();
}

int main()
{
    TrimFileExample(false);
    TrimFileExample(true);
    NewFileExample();
}
