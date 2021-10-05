#include <copc-lib/copc/extents.hpp>

namespace copc
{

CopcExtent::CopcExtent(double minimum, double maximum) : las::CopcExtentsVlr::CopcExtent(minimum, maximum)
{
    if (minimum > maximum)
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
}

CopcExtent::CopcExtent(const std::vector<double> &vec) : las::CopcExtentsVlr::CopcExtent(0, 0)
{
    if (vec.size() != 2)
        throw std::runtime_error("CopcExtent: Vector size must be 2.");

    if (vec[0] > vec[1])
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    minimum = vec[0];
    maximum = vec[1];
}

CopcExtent::CopcExtent(const las::CopcExtentsVlr::CopcExtent &other)
    : las::CopcExtentsVlr::CopcExtent(other.minimum, other.maximum)
{
    if (other.minimum > other.maximum)
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
}

CopcExtents::CopcExtents(int8_t point_format_id, uint16_t eb_count) : point_format_id_(point_format_id)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    extents_ = std::vector<std::shared_ptr<CopcExtent>>(NumberOfExtents(point_format_id, eb_count),
                                                        std::make_shared<CopcExtent>());
}

CopcExtents::CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t eb_count)
    : point_format_id_(point_format_id)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    if (vlr.items.size() != NumberOfExtents(point_format_id, eb_count))
        throw std::runtime_error("CopcExtents: Number of extents incorrect.");
    extents_.reserve(NumberOfExtents(point_format_id, eb_count));
    for (const auto &extent : vlr.items)
    {
        extents_.push_back(std::make_shared<CopcExtent>(extent));
    }
}

las::CopcExtentsVlr CopcExtents::ToCopcExtentsVlr() const
{
    las::CopcExtentsVlr vlr;
    vlr.items.reserve(extents_.size());

    for (auto extent : extents_)
        vlr.items.push_back(extent->ToLazPerf());

    return vlr;
}

int CopcExtents::NumberOfExtents(int8_t point_format_id, uint16_t eb_count)
{
    return las::PointBaseNumberDimensions(point_format_id) + eb_count;
}

size_t CopcExtents::GetByteSize(int8_t point_format_id, uint16_t eb_count)
{
    return CopcExtents(point_format_id, eb_count).ToCopcExtentsVlr().size();
}

} // namespace copc
