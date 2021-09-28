#include <copc-lib/copc/extents.hpp>

namespace copc
{

CopcExtents::CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_extra_bytes)
    : point_format_id(point_format_id), extra_bytes(num_extra_bytes, {0, 0})
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("Supported point formats are 6 to 8.");

    if (vlr.size() != NumberOfExtents(point_format_id, extra_bytes.size()))
        throw std::runtime_error("Number of extents incorrect.");

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
void CopcExtents::SetCopcExtents(const std::vector<las::CopcExtent> &extents)
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

int CopcExtents::NumberOfExtents(int8_t point_format_id, uint16_t num_extra_bytes)
{
    return las::PointBaseNumberDimensions(point_format_id) + num_extra_bytes;
}

} // namespace copc
