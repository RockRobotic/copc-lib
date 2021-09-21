#include <copc-lib/las/point.hpp>
#include <copc-lib/las/utils.hpp>

namespace copc::las
{
Point::Point(const int8_t &point_format_id, const Vector3 &scale, const Vector3 &offset,
             const uint16_t &num_extra_bytes)
    : scale_(scale), offset_(offset), point_format_id_(point_format_id)
{
    if (point_format_id > 10)
        throw std::runtime_error("Point format must be 0-10");

    if (point_format_id > 5)
        extended_point_type_ = true;

    point_record_length_ = PointBaseByteSize(point_format_id) + num_extra_bytes;

    has_gps_time_ = FormatHasGPSTime(point_format_id);
    has_rgb_ = FormatHasRGB(point_format_id);
    has_nir_ = FormatHasNIR(point_format_id);

    extra_bytes_.resize(num_extra_bytes, 0);
}

Point::Point(const LasHeader &header)
    : Point(header.point_format_id, header.scale, header.offset, header.NumExtraBytes()){};

Point::Point(const Point &other) : Point(other.point_format_id_, other.scale_, other.offset_, other.NumExtraBytes())
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

uint16_t Point::NumExtraBytes() const { return point_record_length_ - PointBaseByteSize(point_format_id_); }

bool Point::operator==(const Point &other) const
{
    if (point_format_id_ != other.point_format_id_ || point_record_length_ != other.point_record_length_)
        return false;
    if (x_ != other.UnscaledX() || y_ != other.UnscaledY() || z_ != other.UnscaledZ() ||
        intensity_ != other.Intensity())
        return false;
    if (ReturnNumber() != other.ReturnNumber() || NumberOfReturns() != other.NumberOfReturns())
        return false;
    if (ScanDirectionFlag() != other.ScanDirectionFlag() || EdgeOfFlightLineFlag() != other.EdgeOfFlightLineFlag())
        return false;
    if (Classification() != other.Classification())
        return false;
    if (Synthetic() != other.Synthetic() || KeyPoint() != other.KeyPoint() || Withheld() != other.Withheld())
        return false;
    if (ScanAngle() != other.ScanAngle() || user_data_ != other.UserData() || point_source_id_ != other.PointSourceID())
        return false;
    if (ExtraBytes() != other.ExtraBytes())
        return false;
    if (extended_point_type_ && (Overlap() != other.Overlap() || ScannerChannel() != other.ScannerChannel()))
        return false;
    if (has_gps_time_ && (gps_time_ != other.GPSTime()))
        return false;
    if (has_rgb_ && (rgb_[0] != other.Red() || rgb_[1] != other.Green() || rgb_[2] != other.Blue()))
        return false;
    if (has_nir_ && (nir_ != other.NIR()))
        return false;
    return true;
}

std::shared_ptr<Point> Point::Unpack(std::istream &in_stream, const int8_t &point_format_id, const Vector3 &scale,
                                     const Vector3 &offset, const uint16_t &num_extra_bytes)
{
    std::shared_ptr<Point> p = std::make_shared<Point>(point_format_id, scale, offset, num_extra_bytes);

    p->x_ = unpack<int32_t>(in_stream);
    p->y_ = unpack<int32_t>(in_stream);
    p->z_ = unpack<int32_t>(in_stream);
    p->intensity_ = unpack<uint16_t>(in_stream);
    if (p->extended_point_type_)
    {
        p->extended_returns_ = unpack<uint8_t>(in_stream);
        p->extended_flags_ = unpack<uint8_t>(in_stream);
        p->extended_classification_ = unpack<uint8_t>(in_stream);
        p->user_data_ = unpack<uint8_t>(in_stream);
        p->extended_scan_angle_ = unpack<int16_t>(in_stream);
    }
    else
    {
        p->returns_flags_eof_ = unpack<uint8_t>(in_stream);
        p->classification_ = unpack<uint8_t>(in_stream);
        p->scan_angle_rank_ = unpack<int8_t>(in_stream);
        p->user_data_ = unpack<uint8_t>(in_stream);
    }
    p->point_source_id_ = unpack<uint16_t>(in_stream);

    if (p->has_gps_time_)
    {
        p->gps_time_ = unpack<double>(in_stream);
    }
    if (p->has_rgb_)
    {
        p->rgb_[0] = unpack<uint16_t>(in_stream);
        p->rgb_[1] = unpack<uint16_t>(in_stream);
        p->rgb_[2] = unpack<uint16_t>(in_stream);
    }
    if (p->has_nir_)
    {
        p->nir_ = unpack<uint16_t>(in_stream);
    }

    for (uint32_t i = 0; i < num_extra_bytes; i++)
    {
        p->extra_bytes_[i] = unpack<uint8_t>(in_stream);
    }
    return p;
}

void Point::Pack(std::ostream &out_stream) const
{
    // Point
    pack(x_, out_stream);
    pack(y_, out_stream);
    pack(z_, out_stream);
    pack(intensity_, out_stream);
    if (extended_point_type_)
    {
        pack(extended_returns_, out_stream);
        pack(extended_flags_, out_stream);
        pack(extended_classification_, out_stream);
        pack(user_data_, out_stream);
        pack(extended_scan_angle_, out_stream);
    }
    else
    {
        pack(returns_flags_eof_, out_stream);
        pack(classification_, out_stream);
        pack(scan_angle_rank_, out_stream);
        pack(user_data_, out_stream);
    }
    pack(point_source_id_, out_stream);

    if (has_gps_time_)
        pack(gps_time_, out_stream);
    if (has_rgb_)
    {
        pack(rgb_[0], out_stream);
        pack(rgb_[1], out_stream);
        pack(rgb_[2], out_stream);
    }
    if (has_nir_)
        pack(nir_, out_stream);

    for (auto eb : extra_bytes_)
        pack(eb, out_stream);
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
    point_record_length_ = PointBaseByteSize(point_format_id) + extra_bytes_.size();
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
        ss << "\tRGB: [ " << rgb_[0] << ", " << rgb_[1] << ", " << rgb_[2] << " ]" << std::endl;
    if (has_nir_)
        ss << "\tNIR: " << nir_ << std::endl;
    if (!extra_bytes_.empty())
        ss << "\tExtra Bytes: " << extra_bytes_.size() << std::endl;

    return ss.str();
}

} // namespace copc::las
