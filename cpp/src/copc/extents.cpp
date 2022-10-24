#include "copc-lib/copc/extents.hpp"

#include <sstream>

namespace copc
{

CopcExtent::CopcExtent(double minimum, double maximum, double mean, double var)
    : minimum(minimum), maximum(maximum), mean(mean), var(var)
{
    if (minimum > maximum)
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    if (var < 0)
        throw std::runtime_error("CopcExtent: Variance must be >= 0.");
}

CopcExtent::CopcExtent(const std::vector<double> &vec)
{
    if (vec.size() != 2 && vec.size() != 4)
        throw std::runtime_error("CopcExtent: Vector size must be 2 or 4.");

    if (vec[0] > vec[1])
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    minimum = vec[0];
    maximum = vec[1];

    if (vec.size() == 4)
    {
        mean = vec[2];
        var = vec[3];
    }
}

CopcExtent::CopcExtent(const CopcExtent &other)
    : minimum(other.minimum), maximum(other.maximum), mean(other.mean), var(other.var)
{
    if (other.minimum > other.maximum)
        throw std::runtime_error("CopcExtent: Minimum value must be less or equal than maximum value.");
    if (var < 0)
        throw std::runtime_error("CopcExtent: Variance must be >= 0.");
}

std::string CopcExtent::ToString() const
{
    std::stringstream ss;
    ss << "(" << minimum << "/" << maximum << "/" << mean << "/" << var << ")";
    return ss.str();
}

// Empty constructor
CopcExtents::CopcExtents(int8_t point_format_id, uint16_t num_eb_items, bool has_extended_stats)
    : point_format_id_(point_format_id), has_extended_stats_(has_extended_stats)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");
    auto num_extents = NumberOfExtents(point_format_id, num_eb_items);
    extents_.reserve(num_extents);
    for (int i{0}; i < num_extents; i++)
        extents_.push_back(std::make_shared<CopcExtent>());
}

// Copy constructor
CopcExtents::CopcExtents(const CopcExtents &other)
    : point_format_id_(other.PointFormatId()), has_extended_stats_(other.HasExtendedStats())
{
    extents_.reserve(other.NumberOfExtents());
    for (int i{0}; i < other.NumberOfExtents(); i++)
        extents_.push_back(std::make_shared<CopcExtent>(other.Extents()[i]));
}

CopcExtents::CopcExtents(const CopcExtents &other, int8_t point_format_id, uint16_t num_eb_items,
                         bool has_extended_stats)
    : CopcExtents(point_format_id, num_eb_items, has_extended_stats)
{
    // Copy generic extents
    for (int i{0}; i <= 13; i++)
        extents_[i] = other.extents_[i];

    // Copy RGB
    if (point_format_id > 6 && other.point_format_id_ > 6)
    {
        extents_[14] = other.extents_[14];
        extents_[15] = other.extents_[15];
        extents_[16] = other.extents_[16];
    }

    // Copy NIR
    if (point_format_id == 8 && other.point_format_id_ == 8)
        extents_[17] = other.extents_[17];

    // Copy EBs
    auto other_num_eb = other.NumberOfExtents() - PointBaseNumberExtents(other.PointFormatId());
    if (num_eb_items != other_num_eb)
    {
        std::cout << "CopcExtents: Warning, number of extra byte has changed, can't copy values over" << std::endl;
    }
    else
    {
        for (int i{0}; i < num_eb_items; i++)
            extents_[PointBaseNumberExtents(point_format_id_) + i] =
                other.extents_[PointBaseNumberExtents(other.PointFormatId()) + i];
    }
}

// VLR constructor
CopcExtents::CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_eb_items,
                         bool has_extended_stats)
    : point_format_id_(point_format_id), has_extended_stats_(has_extended_stats)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    if (vlr.items.size() != NumberOfExtents(point_format_id, num_eb_items))
        throw std::runtime_error("CopcExtents: Number of extents incorrect.");
    extents_.reserve(NumberOfExtents(point_format_id, num_eb_items));
    for (int i = 0; i < vlr.items.size(); i++)
    {
        extents_.push_back(std::make_shared<CopcExtent>(vlr.items[i].minimum, vlr.items[i].maximum));
    }
}

las::CopcExtentsVlr CopcExtents::ToLazPerf(const CopcExtent &x, const CopcExtent &y, const CopcExtent &z) const
{
    las::CopcExtentsVlr vlr;
    vlr.items.reserve(extents_.size());
    for (const auto &extent : extents_)
        vlr.items.emplace_back(extent->minimum, extent->maximum);

    return vlr;
}

las::CopcExtentsVlr CopcExtents::ToLazPerfExtended() const
{
    las::CopcExtentsVlr vlr;
    vlr.items.reserve(extents_.size());
    for (const auto &extent : extents_)
        vlr.items.emplace_back(extent->mean, extent->var); // Add mean/var instead of min/max
    return vlr;
}

void CopcExtents::SetExtendedStats(const las::CopcExtentsVlr &vlr)
{
    if (!has_extended_stats_)
        throw std::runtime_error("CopcExtents::SetExtendedStats: This instance does not have extended stats.");
    if (vlr.items.size() != extents_.size())
        throw std::runtime_error("CopcExtents::SetExtendedStats: Number of extended extents incorrect.");
    for (int i = 0; i < vlr.items.size(); i++)
    {
        extents_[i]->mean = vlr.items[i].minimum;
        extents_[i]->var = vlr.items[i].maximum;
    }
}

int CopcExtents::NumberOfExtents(int8_t point_format_id, uint16_t num_eb_items)
{
    return PointBaseNumberExtents(point_format_id) + num_eb_items;
}

size_t CopcExtents::ByteSize(int8_t point_format_id, uint16_t num_eb_items)
{
    return CopcExtents(point_format_id, num_eb_items).ToLazPerf({}, {}, {}).size();
}

std::string CopcExtents::ToString() const
{
    std::stringstream ss;
    ss << "Copc Extents (Min/Max/Mean/Var):" << std::endl;
    ss << "\tX: " << *extents_[0] << std::endl;
    ss << "\tY: " << *extents_[1] << std::endl;
    ss << "\tZ: " << *extents_[2] << std::endl;
    ss << "\tIntensity: " << *extents_[3] << std::endl;
    ss << "\tReturn Number: " << *extents_[4] << std::endl;
    ss << "\tNumber Of Returns: " << *extents_[5] << std::endl;
    ss << "\tScanner Channel: " << *extents_[6] << std::endl;
    ss << "\tScan Direction Flag: " << *extents_[7] << std::endl;
    ss << "\tEdge Of Flight Line: " << *extents_[8] << std::endl;
    ss << "\tClassification: " << *extents_[9] << std::endl;
    ss << "\tUser Data: " << *extents_[10] << std::endl;
    ss << "\tScan Angle: " << *extents_[11] << std::endl;
    ss << "\tPoint Source ID: " << *extents_[12] << std::endl;
    ss << "\tGPS Time: " << *extents_[13] << std::endl;
    if (point_format_id_ > 6)
    {
        ss << "\tRed: " << *extents_[14] << std::endl;
        ss << "\tGreen: " << *extents_[15] << std::endl;
        ss << "\tBlue: " << *extents_[16] << std::endl;
    }
    if (point_format_id_ == 8)
        ss << "\tNIR: " << *extents_[17] << std::endl;
    ss << "\tExtra Bytes:" << std::endl;
    for (int i = PointBaseNumberExtents(point_format_id_); i < extents_.size(); i++)
    {
        ss << "\t\t" << *extents_[i] << std::endl;
    }
    return ss.str();
}

uint8_t PointBaseNumberExtents(const int8_t &point_format_id)
{
    return las::PointBaseNumberDimensions(point_format_id);
}

} // namespace copc
