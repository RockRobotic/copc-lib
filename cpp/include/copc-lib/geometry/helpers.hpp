#ifndef CPP_INCLUDE_COPC_LIB_GEOMETRY_HELPERS
#define CPP_INCLUDE_COPC_LIB_GEOMETRY_HELPERS

#include <cmath>
#include <vector>

namespace copc
{

namespace las
{
class LasHeader;
}
class Box;

enum RoundStrategy
{
    NEAREST,
    PREFER_LARGER_TILE,
    PREFER_SMALLER_TILE
};

// Returns the nearest depth to the requested tile size
static const double GetNearestDepth(double tile_size, const las::LasHeader &header,
                                    RoundStrategy rounding = RoundStrategy::NEAREST);

std::vector<copc::Box> GetPossibleTilesAtDepth(int32_t target_depth, const las::LasHeader &header);

} // namespace copc

#endif /* CPP_INCLUDE_COPC_LIB_GEOMETRY_HELPERS */
