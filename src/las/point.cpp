#include <copc-lib/las/point.hpp>
#include <copc-lib/las/utils.hpp>

namespace copc::las
{
Point::Point(const int8_t &point_format_id, const uint16_t &num_extra_bytes)
{
    if (point_format_id > 10)
        throw std::runtime_error("Point format must be 0-10");

    if (point_format_id > 5)
        extended_point_type_ = true;

    point_format_id_ = point_format_id;
    point_record_length_ = BaseByteSize(point_format_id) + num_extra_bytes;

    has_gps_time_ = FormatHasGPSTime(point_format_id);
    has_rgb_ = FormatHasRGB(point_format_id);
    has_nir_ = FormatHasNIR(point_format_id);

    extra_bytes_.resize(num_extra_bytes, 0);
}

Point::Point(const Point &other) : Point(other.point_format_id_, other.NumExtraBytes())
{
    x_ = other.x_;
    y_ = other.y_;
    z_ = other.z_;
    intensity_ = other.intensity_;
    if (other.extended_point_type_)
    {
        extended_returns_ = other.extended_returns_;
        extended_flags_ = other.extended_flags_;
        extended_classification_ = other.extended_classification_;
        extended_scan_angle_ = other.extended_scan_angle_;
    }
    else
    {
        returns_flags_eof_ = other.returns_flags_eof_;
        classification_ = other.classification_;
        scan_angle_rank_ = other.scan_angle_rank_;
    }
    user_data_ = other.user_data_;
    point_source_id_ = other.point_source_id_;

    if (other.has_gps_time_)
    {
        gps_time_ = other.gps_time_;
    }
    if (other.has_rgb_)
    {
        rgb_[0] = other.Red();
        rgb_[1] = other.Green();
        rgb_[2] = other.Blue();
    }
    if (has_nir_)
    {
        nir_ = other.nir_;
    }

    extra_bytes_ = other.extra_bytes_;
}

Point Point::Unpack(std::istream &in_stream, const int8_t &point_format_id, const uint16_t &num_extra_bytes)
{
    Point p(point_format_id, num_extra_bytes);

    p.x_ = internal::unpack<int32_t>(in_stream);
    p.y_ = internal::unpack<int32_t>(in_stream);
    p.z_ = internal::unpack<int32_t>(in_stream);
    p.intensity_ = internal::unpack<uint16_t>(in_stream);
    if (p.extended_point_type_)
    {
        p.extended_returns_ = internal::unpack<uint8_t>(in_stream);
        p.extended_flags_ = internal::unpack<uint8_t>(in_stream);
        p.extended_classification_ = internal::unpack<uint8_t>(in_stream);
        p.user_data_ = internal::unpack<uint8_t>(in_stream);
        p.extended_scan_angle_ = internal::unpack<int16_t>(in_stream);
    }
    else
    {
        p.returns_flags_eof_ = internal::unpack<uint8_t>(in_stream);
        p.classification_ = internal::unpack<uint8_t>(in_stream);
        p.scan_angle_rank_ = internal::unpack<int8_t>(in_stream);
        p.user_data_ = internal::unpack<uint8_t>(in_stream);
    }
    p.point_source_id_ = internal::unpack<uint16_t>(in_stream);

    if (p.has_gps_time_)
    {
        p.gps_time_ = internal::unpack<double>(in_stream);
    }
    if (p.has_rgb_)
    {
        p.rgb_[0] = internal::unpack<uint16_t>(in_stream);
        p.rgb_[1] = internal::unpack<uint16_t>(in_stream);
        p.rgb_[2] = internal::unpack<uint16_t>(in_stream);
    }
    if (p.has_nir_)
    {
        p.nir_ = internal::unpack<uint16_t>(in_stream);
    }

    for (uint32_t i = 0; i < num_extra_bytes; i++)
    {
        p.extra_bytes_[i] = internal::unpack<uint8_t>(in_stream);
    }
    return p;
}

void Point::Pack(std::ostream &out_stream) const
{

    // Point
    internal::pack(x_, out_stream);
    internal::pack(y_, out_stream);
    internal::pack(z_, out_stream);
    internal::pack(intensity_, out_stream);
    if (extended_point_type_)
    {
        internal::pack(extended_returns_, out_stream);
        internal::pack(extended_flags_, out_stream);
        internal::pack(extended_classification_, out_stream);
        internal::pack(user_data_, out_stream);
        internal::pack(extended_scan_angle_, out_stream);
    }
    else
    {
        internal::pack(returns_flags_eof_, out_stream);
        internal::pack(classification_, out_stream);
        internal::pack(scan_angle_rank_, out_stream);
        internal::pack(user_data_, out_stream);
    }
    internal::pack(point_source_id_, out_stream);

    if (has_gps_time_)
        internal::pack(gps_time_, out_stream);
    if (has_rgb_)
    {
        internal::pack(rgb_[0], out_stream);
        internal::pack(rgb_[1], out_stream);
        internal::pack(rgb_[2], out_stream);
    }
    if (has_nir_)
        internal::pack(nir_, out_stream);

    for (auto eb : extra_bytes_)
        internal::pack(eb, out_stream);
}

uint8_t Point::BaseByteSize(const int8_t &point_format_id)
{
    switch (point_format_id)
    {
    case 0:
        return 20;
    case 1:
        return 28;
    case 2:
        return 26;
    case 3:
        return 34;
    case 4:
        //        return 28;
    case 5:
        //        return 34;
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    case 6:
        return 30;
    case 7:
        return 36;
    case 8:
        return 38;
    case 9:
        //        return 30;
    case 10:
        //        return 38;
        throw std::runtime_error("Point formats with Wave Packets not yet supported");
    default:
        throw std::runtime_error("Point format must be 0-10");
    }
}

void Point::ToPointFormat(const int8_t &point_format_id)
{

    if (extended_point_type_ && point_format_id < 6)
    {
        // Convert points values from 1.4 to 1.0
        // Returns and flags
        returns_flags_eof_ = (EdgeOfFlightLineFlag() << 7) | (ScanDirectionFlag() << 6) |
                             (std::min(int(NumberOfReturns()), 7) << 3) | std::min(int(ReturnNumber()), 7);
        // Classification
        classification_ =
            (Withheld() << 7) | (KeyPoint() << 6) | (Synthetic() << 5) | std::min(int(Classification()), 31);

        // Scan angle
        scan_angle_rank_ = static_cast<int8_t>(std::max(-90, std::min(90, int(float(ExtendedScanAngle()) * 0.006f))));
    }
    else if (!extended_point_type_ && point_format_id > 5)
    {
        // Convert point values from 1.0 to 1.4
        // Returns
        extended_returns_ = (NumberOfReturns() << 4) | ReturnNumber();
        // Flags
        extended_flags_ = (EdgeOfFlightLineFlag() << 7) | (ScanDirectionFlag() << 6) | (Withheld() << 2) |
                          (KeyPoint() << 1) | Synthetic();
        // Classification
        extended_classification_ = Classification();
        // Scan angle
        extended_scan_angle_ = static_cast<int16_t>(float(ScanAngleRank()) / 0.006f);
    }

    // Update flags
    extended_point_type_ = point_format_id > 5;
    has_gps_time_ = FormatHasGPSTime(point_format_id);
    has_rgb_ = FormatHasRGB(point_format_id);
    has_nir_ = FormatHasNIR(point_format_id);
    point_record_length_ = BaseByteSize(point_format_id) + extra_bytes_.size();
    point_format_id_ = point_format_id;
}

std::string Point::ToString() const
{
    std::stringstream ss;
    ss << "Point: " << std::endl;
    if (extended_point_type_)
    {
        ss << "\tPoint14: " << std::endl;
        ss << "\t\tX: " << x_ << ", Y: " << y_ << ", Z: " << z_ << std::endl;
        ss << "\t\tIntensity: " << intensity_ << std::endl;
        ss << "\t\tReturn Number: " << static_cast<short>(ReturnNumber())
           << ", Number of Returns: " << static_cast<short>(NumberOfReturns()) << std::endl;
        ss << "\t\tClassification Flags: Synthetic: " << Synthetic() << ", Key Point: " << KeyPoint()
           << ", Withheld: " << Withheld() << ", Overlap: " << Overlap() << std::endl;
        ss << "\t\tScannerChannel: " << static_cast<short>(ScannerChannel()) << std::endl;
        ss << "\t\tScan Direction: " << ScanDirectionFlag() << std::endl;
        ss << "\t\tEdge of Flight Line: " << EdgeOfFlightLineFlag() << std::endl;
        ss << "\t\tClasification: " << static_cast<short>(Classification()) << std::endl;
        ss << "\t\tUser Data: " << static_cast<short>(user_data_) << std::endl;
        ss << "\t\tScan Angle: " << extended_scan_angle_ << std::endl;
        ss << "\t\tPoint Source ID: " << point_source_id_ << std::endl;
    }
    else
    {
        ss << "\tPoint10: " << std::endl;
        ss << "\t\tX: " << x_ << ", Y: " << y_ << ", Z: " << z_ << std::endl;
        ss << "\t\tIntensity: " << intensity_ << std::endl;
        ss << "\t\tReturn Number: " << static_cast<short>(ReturnNumber())
           << ", Number of Returns: " << static_cast<short>(NumberOfReturns()) << std::endl;
        ss << "\t\tScan Direction: " << ScanDirectionFlag() << std::endl;
        ss << "\t\tEdge of Flight Line: " << EdgeOfFlightLineFlag() << std::endl;
        ss << "\t\tClasification: " << static_cast<short>(Classification()) << std::endl;
        ss << "\t\tClassification Flags: Synthetic: " << Synthetic() << ", Key Point: " << KeyPoint()
           << ", Withheld: " << Withheld() << std::endl;
        ss << "\t\tScan Angle Rank: " << static_cast<short>(scan_angle_rank_) << std::endl;
        ss << "\t\tUser Data: " << static_cast<short>(user_data_) << std::endl;
        ss << "\t\tPoint Source ID: " << point_source_id_ << std::endl;
    }
    if (has_gps_time_)
        ss << "\tGPS Time: " << gps_time_ << std::endl;
    if (has_rgb_)
        ss << "\tRGB: [" << rgb_[0] << "," << rgb_[1] << "," << rgb_[2] << "]" << std::endl;
    if (has_nir_)
        ss << "\tNIR: " << nir_ << std::endl;
    if (!extra_bytes_.empty())
        ss << "\tExtra Bytes: " << extra_bytes_.size() << std::endl;

    return ss.str();
}

} // namespace copc::las
