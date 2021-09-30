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

CopcExtents::CopcExtents(int8_t point_format_id, uint16_t num_extra_bytes)
    : point_format_id(point_format_id), extra_bytes(num_extra_bytes, {0, 0})
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");
}

CopcExtents::CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_extra_bytes)
    : point_format_id(point_format_id), extra_bytes(num_extra_bytes, {0, 0})
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("CopcExtents: Supported point formats are 6 to 8.");

    if (vlr.items.size() != NumberOfExtents(point_format_id, extra_bytes.size()))
        throw std::runtime_error("CopcExtents: Number of extents incorrect.");

    x = vlr.items[0];
    y = vlr.items[1];
    z = vlr.items[2];
    intensity = vlr.items[3];
    return_number = vlr.items[4];
    number_of_returns = vlr.items[5];
    scanner_channel = vlr.items[6];
    scan_direction_flag = vlr.items[7];
    edge_of_flight_line = vlr.items[8];
    classification = vlr.items[9];
    user_data = vlr.items[10];
    scan_angle = vlr.items[11];
    point_source_id = vlr.items[12];
    gps_time = vlr.items[13];
    int eb_start_id = 14;

    if (point_format_id > 6)
    {
        red = vlr.items[14];
        green = vlr.items[15];
        blue = vlr.items[16];
        eb_start_id = 17;
    }
    if (point_format_id == 8)
    {
        nir = vlr.items[17];
        eb_start_id = 18;
    }
    extra_bytes.assign(vlr.items.begin() + eb_start_id, vlr.items.end());
}

las::CopcExtentsVlr CopcExtents::ToCopcExtentsVlr() const
{
    las::CopcExtentsVlr vlr;
    vlr.items.reserve(NumberOfExtents(point_format_id, extra_bytes.size()));
    vlr.items.push_back(x.ToLazPerf());
    vlr.items.push_back(y.ToLazPerf());
    vlr.items.push_back(z.ToLazPerf());
    vlr.items.push_back(intensity.ToLazPerf());
    vlr.items.push_back(return_number.ToLazPerf());
    vlr.items.push_back(number_of_returns.ToLazPerf());
    vlr.items.push_back(scanner_channel.ToLazPerf());
    vlr.items.push_back(scan_direction_flag.ToLazPerf());
    vlr.items.push_back(edge_of_flight_line.ToLazPerf());
    vlr.items.push_back(classification.ToLazPerf());
    vlr.items.push_back(user_data.ToLazPerf());
    vlr.items.push_back(scan_angle.ToLazPerf());
    vlr.items.push_back(point_source_id.ToLazPerf());
    vlr.items.push_back(gps_time.ToLazPerf());

    if (point_format_id > 6)
    {
        vlr.items.push_back(red.ToLazPerf());
        vlr.items.push_back(green.ToLazPerf());
        vlr.items.push_back(blue.ToLazPerf());
    }
    if (point_format_id == 8)
    {
        vlr.items.push_back(nir.ToLazPerf());
    }

    for (const auto &extent : extra_bytes)
        vlr.items.push_back(extent.ToLazPerf());
    return vlr;
}
void CopcExtents::Extents(const std::vector<CopcExtent> &extents)
{
    if (extents.size() != NumberOfExtents(point_format_id, extra_bytes.size()))
        throw std::runtime_error("Extents: Number of extents incorrect.");

    x = extents[0];
    y = extents[1];
    z = extents[2];
    intensity = extents[3];
    return_number = extents[4];
    number_of_returns = extents[5];
    scanner_channel = extents[6];
    scan_direction_flag = extents[7];
    edge_of_flight_line = extents[8];
    classification = extents[9];
    user_data = extents[10];
    scan_angle = extents[11];
    point_source_id = extents[12];
    gps_time = extents[13];
    int eb_start_id = 14;

    if (point_format_id > 6)
    {
        red = extents[14];
        green = extents[15];
        blue = extents[16];
        eb_start_id = 17;
    }
    if (point_format_id == 8)
    {
        nir = extents[17];
        eb_start_id = 18;
    }
    extra_bytes.assign(extents.begin() + eb_start_id, extents.end());
}

std::vector<CopcExtent> CopcExtents::Extents() const
{
    std::vector<CopcExtent> extents(NumberOfExtents(point_format_id, extra_bytes.size()));

    extents[0] = x;
    extents[1] = y;
    extents[2] = z;
    extents[3] = intensity;
    extents[4] = return_number;
    extents[5] = number_of_returns;
    extents[6] = scanner_channel;
    extents[7] = scan_direction_flag;
    extents[8] = edge_of_flight_line;
    extents[9] = classification;
    extents[10] = user_data;
    extents[11] = scan_angle;
    extents[12] = point_source_id;
    extents[13] = gps_time;
    int eb_start_id = 14;

    if (point_format_id > 6)
    {
        extents[14] = red;
        extents[15] = green;
        extents[16] = blue;
        eb_start_id = 17;
    }
    if (point_format_id == 8)
    {
        extents[17] = nir;
        eb_start_id = 18;
    }
    std::copy(extra_bytes.begin(), extra_bytes.end(), extents.begin() + eb_start_id);
    return extents;
}

int CopcExtents::NumberOfExtents(int8_t point_format_id, uint16_t num_extra_bytes)
{
    return las::PointBaseNumberDimensions(point_format_id) + num_extra_bytes;
}

} // namespace copc
