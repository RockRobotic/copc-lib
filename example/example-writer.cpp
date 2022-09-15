#include <cassert>
#include <cmath>
#include <random>

#include <copc-lib/geometry/vector3.hpp>
// #include <copc-lib/hierarchy/key.hpp>
// #include <copc-lib/io/copc_reader.hpp>
// #include <copc-lib/io/copc_writer.hpp>
// #include <copc-lib/las/header.hpp>
// #include <copc-lib/laz/compressor.hpp>
// #include <copc-lib/laz/decompressor.hpp>

using namespace copc;
using namespace std;

// // In this example, we'll filter the autzen dataset to only contain depth levels 0-3.
// void TrimFileExample(bool compressor_example_flag)
// {
//     // We'll get our point data from this file
//     FileReader reader("autzen-classified.copc.laz");
//     auto old_header = reader.CopcConfig().LasHeader();

//     {
//         // Copy the config to the new file
//         auto cfg = reader.CopcConfig();

//         // Now, we can create our actual writer
//         FileWriter writer("autzen-trimmed.copc.laz", cfg);

//         // GetAllChildrenOfPage will load the entire hierarchy under a given key
//         for (const auto &node : reader.GetAllChildrenOfPage(VoxelKey::RootKey()))
//         {
//             // In this example, we'll only save up to depth level 3.
//             if (node.key.d > 3)
//                 continue;

//             if (!compressor_example_flag)
//             {
//                 // It's much faster to write and read compressed data, to avoid compression and decompression
//                 writer.AddNodeCompressed(
//                     node.key, reader.GetPointDataCompressed(node), node.point_count,
//                     node.page_key); // We can provide the optional page key to preserve the page hierarchy (here
//                     root)
//             }
//             else
//             {
//                 // Alternatively, if we have uncompressed data and want to compress it without writing it to the
//                 file,
//                 // (for example, compress multiple nodes in parallel and have one thread writing the data),
//                 // we can use the Compressor class:

//                 std::vector<char> uncompressed_points = reader.GetPointData(node);
//                 std::vector<char> compressed_points =
//                     laz::Compressor::CompressBytes(uncompressed_points, *writer.CopcConfig()->LasHeader());
//                 writer.AddNodeCompressed(node.key, compressed_points, node.point_count, node.page_key);
//             }
//         }

//         // Make sure we call close to finish writing the file!
//         writer.Close();
//     }

//     // Now, let's test our new file
//     FileReader new_reader("autzen-trimmed.copc.laz");

//     // Let's go through each node we've written and make sure it matches the original
//     for (const auto &node : new_reader.GetAllNodes())
//     {
//         assert(new_reader.GetPointDataCompressed(node) == reader.GetPointDataCompressed(node.key));

//         // Similarly, we could retrieve the compressed node data from the file
//         // and decompress it later using the Decompressor class
//         if (compressor_example_flag)
//         {
//             las::LasHeader header = new_reader.CopcConfig().LasHeader();
//             std::vector<char> compressed_points = reader.GetPointDataCompressed(node.key);
//             std::vector<char> uncompressed_points =
//                 laz::Decompressor::DecompressBytes(compressed_points, header, node.point_count);
//         }
//     }
// }

// // In this example, we'll filter the points in the autzen dataset based on bounds.
// void BoundsTrimFileExample()
// {
//     // We'll get our point data from this file
//     FileReader reader("autzen-classified.copc.laz");
//     auto old_header = reader.CopcConfig().LasHeader();

//     // Take horizontal 2D box of [400,400] roughly in the middle of the point cloud.

//     auto middle = (old_header.max + old_header.min) / 2;
//     Box box(middle.x - 200, middle.y - 200, middle.x + 200, middle.y + 200);

//     {
//         // Copy the config to the new file
//         auto cfg = reader.CopcConfig();

//         // Now, we can create our actual writer, here we will update the Point Format ID in the new file to be 8
//         FileWriter writer("autzen-bounds-trimmed.copc.laz", cfg, 8);

//         for (const auto &node : reader.GetAllNodes())
//         {

//             if (node.key.Within(old_header, box))
//             {
//                 // If node is within the box then add all points (without decompressing)
//                 writer.AddNodeCompressed(node.key, reader.GetPointDataCompressed(node), node.point_count,
//                                          node.page_key);
//             }
//             else if (node.key.Intersects(old_header, box))
//             {
//                 // If node only crosses the box then decompress points data and get subset of points that are within
//                 the
//                 // box
//                 auto point_vector = reader.GetPoints(node).GetWithin(box);
//                 auto points = las::Points(point_vector);
//                 // Here we update the Point Format ID to 8 since we updated the point format ID of the writer to 8
//                 points.ToPointFormat(8);
//                 writer.AddNode(node.key, points, node.page_key);
//             }
//         }

//         // Make sure we call close to finish writing the file!
//         writer.Close();
//     }

//     // Now, let's test our new file
//     FileReader new_reader("autzen-bounds-trimmed.copc.laz");

//     // Let's go through each point and make sure they fit within the Box
//     for (const auto &node : new_reader.GetAllNodes())
//     {
//         auto points = new_reader.GetPoints(node);
//         assert(points.Within(box));
//     }
// }

// // In this example, we'll filter the points in the autzen dataset based on resolution.
// void ResolutionTrimFileExample()
// {
//     // We'll get our point data from this file
//     FileReader reader("autzen-classified.copc.laz");
//     auto old_header = reader.CopcConfig().LasHeader();

//     double resolution = 10;
//     auto target_depth = reader.GetDepthAtResolution(resolution);
//     // Check that the resolution of the target depth is equal or smaller to the requested resolution.
//     assert(VoxelKey::GetResolutionAtDepth(target_depth, old_header, reader.CopcConfig().CopcInfo()) <= resolution);
//     {
//         // Copy the config to the new file
//         auto cfg = reader.CopcConfig();

//         // Now, we can create our actual writer
//         FileWriter writer("autzen-resolution-trimmed.copc.laz", cfg);

//         for (const auto &node : reader.GetAllNodes())
//         {
//             if (node.key.d <= target_depth)
//             {
//                 writer.AddNodeCompressed(node.key, reader.GetPointDataCompressed(node), node.point_count,
//                                          node.page_key);
//             }
//         }

//         // Make sure we call close to finish writing the file!
//         writer.Close();
//     }

//     // Now, let's test our new file
//     FileReader new_reader("autzen-resolution-trimmed.copc.laz");

//     auto new_header = new_reader.CopcConfig().LasHeader();
//     auto new_copc_info = new_reader.CopcConfig().CopcInfo();

//     // Let's go through each node we've written and make sure the resolution is correct
//     for (const auto &node : new_reader.GetAllNodes())
//     {
//         assert(node.key.d <= target_depth);
//     }

//     // Let's make sure the max resolution is at least as much as we requested
//     auto max_octree_depth = new_reader.GetDepthAtResolution(0);
//     assert(VoxelKey::GetResolutionAtDepth(max_octree_depth, new_header, new_copc_info) <= resolution);
// }

// // constants
// const Vector3 MIN_BOUNDS = {-2000, -5000, 20}; // Scaled coordinates
// const Vector3 MAX_BOUNDS = {5000, 1034, 125};  // Scaled coordinates
// const int NUM_POINTS = 3000;

// // random num devices
// std::random_device rd;  // obtain a random number from hardware
// std::mt19937 gen(rd()); // seed the generator

// // This function will generate `NUM_POINTS` random points within the bounds
// las::Points RandomPoints(const VoxelKey &key, const las::LasHeader &header, int number_points)
// {
//     // Voxel cube dimensions will be calculated from the maximum span of the file
//     double span = std::max({MAX_BOUNDS.x - MIN_BOUNDS.x, MAX_BOUNDS.y - MIN_BOUNDS.y, MAX_BOUNDS.z - MIN_BOUNDS.z});
//     // Step size accounts for depth level
//     double step = span / std::pow(2, key.d);

//     double x_min = header.min.x + (step * key.x);
//     double y_min = header.min.y + (step * key.y);
//     double z_min = header.min.z + (step * key.z);

//     // Random num generators between the min and max spatial bounds of the voxel
//     std::uniform_int_distribution<> rand_x(header.RemoveScaleX(std::max(header.min.x, x_min)),
//                                            header.RemoveScaleX(std::min(header.max.x, x_min + step)));
//     std::uniform_int_distribution<> rand_y(header.RemoveScaleY(std::max(header.min.y, y_min)),
//                                            header.RemoveScaleY(std::min(header.max.y, y_min + step)));
//     std::uniform_int_distribution<> rand_z(header.RemoveScaleZ(std::max(header.min.z, z_min)),
//                                            header.RemoveScaleZ(std::min(header.max.z, z_min + step)));

//     // Create a Points object based on the LAS header
//     las::Points points(header);

//     // Populate the points
//     for (int i = 0; i < number_points; i++)
//     {
//         // Create a point with a given point format
//         // The use of las::Point constructor is strongly discouraged, instead use las::Points::CreatePoint
//         auto point = points.CreatePoint();
//         // point has getters/setters for all attributes
//         point->X(header.ApplyScaleX(rand_x(gen)));
//         point->Y(header.ApplyScaleY(rand_y(gen)));
//         point->Z(header.ApplyScaleZ(rand_z(gen)));
//         // For visualization purposes
//         point->PointSourceId(key.d + key.x + key.y + key.z);

//         points.AddPoint(point);
//     }
//     return points;
// }

// // In this example, we'll create our own file from scratch
// void NewFileExample()
// {
//     // Create our new file with the specified format, scale, offset, wkt, and extended_stats
//     CopcConfigWriter cfg(8, {0.1, 0.1, 0.1}, {50, 50, 50}, "TEST_WKT", {}, true);
//     // copc-lib will not automatically compute the min/max of added points
//     // so we will have to calculate it ourselves
//     cfg.LasHeader()->min = MIN_BOUNDS;
//     cfg.LasHeader()->max = MAX_BOUNDS;

//     cfg.CopcInfo()->spacing = 10;
//     // Now, we can create our COPC writer
//     FileWriter writer("new-copc.copc.laz", cfg);
//     auto header = writer.CopcConfig()->LasHeader();

//     // Set the COPC Extents
//     auto extents = writer.CopcConfig()->CopcExtents();

//     extents->Intensity()->minimum = 0;
//     extents->Intensity()->maximum = 10000;
//     extents->Intensity()->mean = 50;
//     extents->Intensity()->var = 5;

//     extents->Classification()->minimum = 5;
//     extents->Classification()->maximum = 201;

//     // First we'll add a root node
//     VoxelKey key(0, 0, 0, 0);
//     auto points = RandomPoints(key, *header, NUM_POINTS);
//     // The node will be written to the file when we call AddNode
//     writer.AddNode(key, points);

//     // We can also add pages, as long as the key we specify is a child of the parent page
//     {
//         auto page_key = VoxelKey(1, 1, 1, 0);

//         // Once our page is created, we can add nodes to it like before
//         key = VoxelKey(1, 1, 1, 0);
//         points = RandomPoints(key, *header, NUM_POINTS);
//         writer.AddNode(key, points, page_key);

//         key = VoxelKey(2, 2, 2, 0);
//         points = RandomPoints(key, *header, NUM_POINTS);
//         writer.AddNode(key, points, page_key);

//         // We can nest subpages as much as we want, as long as they are children of the parent
//         auto sub_page_key = VoxelKey(3, 4, 4, 0);
//         points = RandomPoints(sub_page_key, *header, NUM_POINTS);
//         writer.AddNode(sub_page_key, points, sub_page_key);
//     }

//     // Make sure we call close to finish writing the file!
//     writer.Close();

//     // We can check that the spatial bounds of the file have been respected
//     FileReader reader("new-copc.copc.laz");
//     assert(reader.ValidateSpatialBounds());

//     // We can get the keys of all existing pages
//     auto page_keys = reader.GetPageList();
//     // Check that a page exists
//     assert(std::find(page_keys.begin(), page_keys.end(), VoxelKey(3, 4, 4, 0)) != page_keys.end());

//     // We can get the page of any node (useful to copy the file along with the hierarchy)
//     auto node = reader.FindNode(VoxelKey(2, 2, 2, 0));
//     assert(node.page_key == VoxelKey(1, 1, 1, 0));
// }

#include "copc-lib/io/laz_reader.hpp"
#include "copc-lib/io/laz_writer.hpp"
int main()
{
    string file_path = "writer_test.laz";

    las::LazConfigWriter cfg(6);
    laz::LazFileWriter writer(file_path, cfg);

    // Create a Points object based on the LAS header
    las::Points points(*cfg.LasHeader());
    // Populate the points
    // Create a point with a given point format
    // The use of las::Point constructor is strongly discouraged, instead use las::Points::CreatePoint
    auto point = points.CreatePoint();
    // point has getters/setters for all attributes
    point->X(0);
    point->Y(0);
    point->Z(0);
    points.AddPoint(point);
    point = points.CreatePoint();
    // point has getters/setters for all attributes
    point->X(10);
    point->Y(10);
    point->Z(10);
    point->GPSTime(1000032.23);
    points.AddPoint(point);

    writer.WritePoints(points);
    writer.WritePoints(points);
    writer.Close();

    // Validate
    laz::LazFileReader reader(file_path);
    auto read_points = reader.GetPoints();
    for (int i = 0; i < read_points.Size(); i++)
    {
        std::cout << i << ": " << read_points.Get(i)->ToString() << std::endl;
    }
}
