#include "copc-lib/geometry/helpers.hpp"
#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/header.hpp"

#include <limits>

namespace copc
{

const double GetNearestDepth(double tile_size, const las::LasHeader &header, RoundStrategy rounding)
{
    double depth = std::log2(header.Span() / tile_size);
    int nearest_depth;
    if (rounding == RoundStrategy::NEAREST)
        nearest_depth = std::lround(depth);
    else if (rounding == RoundStrategy::PREFER_LARGER_TILE)
        nearest_depth = (int)std::floor(depth);
    else if (rounding == RoundStrategy::PREFER_SMALLER_TILE)
        nearest_depth = (int)std::ceil(depth);
    return std::max(0, nearest_depth);
}

// Generates a vector of Boxes that tile the dataset in its entirety
// Note that points in the same node may fall within different tiles,
// due to rounding
// Additionally, some boxes may contain no points at all, since
// we cannot know for certain whether a tile has points without actually loading it
const std::vector<copc::Box> GetPossibleTilesAtDepth(int32_t target_depth, const las::LasHeader &header)
{
    std::vector<copc::Box> possible_tiles;

    // We can compute the maximum VoxelKey coordinate based on the depth
    int max_possible_coord = std::pow(2, target_depth);
    // But then, we can limit the search space in each dimension,
    // since not every dimension will use the full span
    // (since the span is based on the axis with the largest range)
    double tile_size = header.Span() / max_possible_coord;
    int max_x_coord = (int)std::ceil((header.max.x - header.min.x) / tile_size);
    int max_y_coord = (int)std::ceil((header.max.y - header.min.y) / tile_size);
    int max_z_coord = (int)std::ceil((header.max.z - header.min.z) / tile_size);

    for (int x = 0; x < max_x_coord; x++)
        for (int y = 0; y < max_y_coord; y++)
            for (int z = 0; z < max_z_coord; z++)
            {
                auto box = copc::Box(copc::VoxelKey(target_depth, x, y, z), header);
                // To avoid the same point falling into two boxes when the point's coordinates
                // are the exact same as the box's limit,
                // we offset the minimum coordinate in each dimension by the smallest possible
                // floating point value so that two neighboring boxes don't have the exact same max and min
                // but, don't do that to the first coordinate in each dimension since there's no other box
                // for that point to fall in
                if (x != 0)
                    box.x_min = std::nextafter(box.x_min, std::numeric_limits<double>::max());
                if (y != 0)
                    box.y_min = std::nextafter(box.y_min, std::numeric_limits<double>::max());
                if (z != 0)
                    box.z_min = std::nextafter(box.z_min, std::numeric_limits<double>::max());
                possible_tiles.push_back(box);
            }
    return possible_tiles;
}

const std::vector<copc::Box> GetPossibleTilesWithSize(double target_tile_size, const las::LasHeader &header)
{
    std::vector<copc::Box> possible_tiles;

    int max_num_tiles = header.Span() / target_tile_size;

    for (int x = 0; x < max_num_tiles; x++)
        for (int y = 0; y < max_num_tiles; y++)
            for (int z = 0; z < max_num_tiles; z++)
            {
                double x_min = target_tile_size * x + header.min.x;
                double y_min = target_tile_size * y + header.min.y;
                double z_min = target_tile_size * z + header.min.z;
                double x_max = x_min + target_tile_size;
                double y_max = y_min + target_tile_size;
                double z_max = z_min + target_tile_size;
                auto box = copc::Box(x_min, y_min, z_min, x_max, y_max, z_max);
                // To avoid the same point falling into two boxes when the point's coordinates
                // are the exact same as the box's limit,
                // we offset the minimum coordinate in each dimension by the smallest possible
                // floating point value so that two neighboring boxes don't have the exact same max and min
                // but, don't do that to the first coordinate in each dimension since there's no other box
                // for that point to fall in
                if (x != 0)
                    box.x_min = std::nextafter(box.x_min, std::numeric_limits<double>::max());
                if (y != 0)
                    box.y_min = std::nextafter(box.y_min, std::numeric_limits<double>::max());
                if (z != 0)
                    box.z_min = std::nextafter(box.z_min, std::numeric_limits<double>::max());
                possible_tiles.push_back(box);
            }
    return possible_tiles;
}

} // namespace copc
