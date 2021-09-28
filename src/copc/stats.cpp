#include <copc-lib/copc/stats.hpp>

namespace copc
{

std::vector<las::CopcExtent> CopcStats::ToCopcExtents() const
{
    std::vector<las::CopcExtent> extents;
    extents.reserve(NumberOfExtents(point_format_id, extra_bytes.size()));

    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("Supported point formats are 6 to 8.");

    extents.push_back(x);
    extents.push_back(y);
    extents.push_back(z);
    extents.push_back(intensity);
    extents.push_back(return_number);
    extents.push_back(number_of_returns);
    extents.push_back(scanner_channel);
    extents.push_back(scan_direction_flag);
    extents.push_back(edge_of_flight_line);
    extents.push_back(classification);
    extents.push_back(user_data);
    extents.push_back(scan_angle);
    extents.push_back(point_source_id);
    extents.push_back(gps_time);

    if (point_format_id > 6)
    {
        extents.push_back(red);
        extents.push_back(green);
        extents.push_back(blue);
    }
    if (point_format_id == 8)
    {
        extents.push_back(nir);
    }

    for (const auto &extent : extra_bytes)
        extents.push_back(extent);

    return extents;
}
void CopcStats::FromCopcExtents(const std::vector<las::CopcExtent> &extents)
{
    if (extents.size() != NumberOfExtents(point_format_id, extra_bytes.size()))
        throw std::runtime_error("Number of extents incorrect.");

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

int CopcStats::NumberOfExtents(int8_t point_format_id, uint16_t num_extra_bytes)
{
    return las::PointBaseNumberDimensions(point_format_id) + num_extra_bytes;
}

} // namespace copc
