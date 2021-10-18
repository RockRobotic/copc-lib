#include "copc-lib/copc/extents.hpp"

#include <sstream>

namespace copc
{

CopcExtent::CopcExtent() : las::CopcExtentsVlr::CopcExtent() {}

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

std::string CopcExtent::ToString() const
{
    std::stringstream ss;
    ss << "(" << minimum << "/" << maximum << ")";
    return ss.str();
}

CopcExtents::CopcExtents(int8_t point_format_id, uint16_t num_eb_items) : point_format_id_(point_format_id)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    extents_ = std::vector<std::shared_ptr<CopcExtent>>(NumberOfExtents(point_format_id, num_eb_items),
                                                        std::make_shared<CopcExtent>());
}

CopcExtents::CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_eb_items)
    : point_format_id_(point_format_id)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    if (vlr.items.size() != NumberOfExtents(point_format_id, num_eb_items))
        throw std::runtime_error("CopcExtents: Number of extents incorrect.");
    extents_.reserve(NumberOfExtents(point_format_id, num_eb_items));
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

int CopcExtents::NumberOfExtents(int8_t point_format_id, uint16_t num_eb_items)
{
    return las::PointBaseNumberDimensions(point_format_id) + num_eb_items;
}

size_t CopcExtents::GetByteSize(int8_t point_format_id, uint16_t num_eb_items)
{
    return CopcExtents(point_format_id, num_eb_items).ToCopcExtentsVlr().size();
}

std::string CopcExtents::ToString() const
{
    std::stringstream ss;
    ss << "Copc Extents (Min/Max):" << std::endl;
    ss << "\tX: " << extents_[0]->ToString() << std::endl;
    ss << "\tY: " << extents_[1]->ToString() << std::endl;
    ss << "\tZ: " << extents_[2]->ToString() << std::endl;
    ss << "\tIntensity: " << extents_[3]->ToString() << std::endl;
    ss << "\tReturn Number: " << extents_[4]->ToString() << std::endl;
    ss << "\tNumber Of Returns: " << extents_[5]->ToString() << std::endl;
    ss << "\tScanner Channel: " << extents_[6]->ToString() << std::endl;
    ss << "\tScan Direction Flag: " << extents_[7]->ToString() << std::endl;
    ss << "\tEdge Of Flight Line: " << extents_[8]->ToString() << std::endl;
    ss << "\tClassification: " << extents_[9]->ToString() << std::endl;
    ss << "\tUser Data: " << extents_[10]->ToString() << std::endl;
    ss << "\tScan Angle: " << extents_[11]->ToString() << std::endl;
    ss << "\tPoint Source ID: " << extents_[12]->ToString() << std::endl;
    ss << "\tGPS Time: " << extents_[13]->ToString() << std::endl;
    if (point_format_id_ > 6)
    {
        ss << "\tRed: " << extents_[14]->ToString() << std::endl;
        ss << "\tGreen: " << extents_[15]->ToString() << std::endl;
        ss << "\tBlue: " << extents_[16]->ToString() << std::endl;
    }
    if (point_format_id_ == 8)
        ss << "\tNIR: " << extents_[17]->ToString() << std::endl;
    ss << "\tExtra Bytes:" << std::endl;
    for (int i = las::PointBaseNumberDimensions(point_format_id_); i < extents_.size(); i++)
    {
        ss << "\t\t" << extents_[i]->ToString() << std::endl;
    }
    return ss.str();
}

} // namespace copc
