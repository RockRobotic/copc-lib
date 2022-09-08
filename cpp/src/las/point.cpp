#include "copc-lib/las/point.hpp"
#include "copc-lib/utils.hpp"

namespace copc::las
{
Point::Point(const int8_t &point_format_id, const uint16_t &eb_byte_size) : point_format_id_(point_format_id)
{
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("Point: Point format must be 6-8");

    point_record_length_ = PointBaseByteSize(point_format_id) + eb_byte_size;

    has_rgb_ = FormatHasRgb(point_format_id);
    has_nir_ = FormatHasNir(point_format_id);

    extra_bytes_.resize(eb_byte_size, 0);
}

Point::Point(const LasHeader &header) : Point(header.PointFormatId(), header.EbByteSize()) {}

Point::Point(const Point &other) : Point(other.point_format_id_, other.EbByteSize())
{
    x_scaled_ = other.x_scaled_;
    y_scaled_ = other.y_scaled_;
    z_scaled_ = other.z_scaled_;
    intensity_ = other.intensity_;
    returns_ = other.returns_;
    flags_ = other.flags_;
    classification_ = other.classification_;
    scan_angle_ = other.scan_angle_;

    user_data_ = other.user_data_;
    point_source_id_ = other.point_source_id_;

    gps_time_ = other.gps_time_;
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

uint16_t Point::EbByteSize() const { return point_record_length_ - PointBaseByteSize(point_format_id_); }

bool Point::operator==(const Point &other) const
{
    if (point_format_id_ != other.point_format_id_ || point_record_length_ != other.point_record_length_)
        return false;
    if (!AreClose(X(), other.X(), 0.000001) || !AreClose(Y(), other.Y(), 0.000001) ||
        !AreClose(Z(), other.Z(), 0.000001) || intensity_ != other.Intensity())
        return false;
    if (returns_ != other.ReturnsBitField())
        return false;
    if (flags_ != other.FlagsBitField())
        return false;
    if (classification_ != other.Classification())
        return false;
    if (scan_angle_ != other.ScanAngle() || user_data_ != other.UserData() || point_source_id_ != other.PointSourceId())
        return false;
    if (extra_bytes_ != other.ExtraBytes())
        return false;
    if (gps_time_ != other.GPSTime())
        return false;
    if (has_rgb_ && (rgb_[0] != other.Red() || rgb_[1] != other.Green() || rgb_[2] != other.Blue()))
        return false;
    if (has_nir_ && (nir_ != other.Nir()))
        return false;
    return true;
}

bool Point::Within(const Box &box) const { return box.Contains(Vector3(X(), Y(), Z())); }

std::shared_ptr<Point> Point::Unpack(std::istream &in_stream, const int8_t &point_format_id, const Vector3 &scale,
                                     const Vector3 &offset, const uint16_t &eb_byte_size)
{
    std::shared_ptr<Point> p = std::make_shared<Point>(point_format_id, eb_byte_size);

    p->x_scaled_ = ApplyScale(unpack<int32_t>(in_stream), scale.x, offset.x);
    p->y_scaled_ = ApplyScale(unpack<int32_t>(in_stream), scale.y, offset.y);
    p->z_scaled_ = ApplyScale(unpack<int32_t>(in_stream), scale.z, offset.z);
    p->intensity_ = unpack<uint16_t>(in_stream);
    p->returns_ = unpack<uint8_t>(in_stream);
    p->flags_ = unpack<uint8_t>(in_stream);
    p->classification_ = unpack<uint8_t>(in_stream);
    p->user_data_ = unpack<uint8_t>(in_stream);
    p->scan_angle_ = unpack<int16_t>(in_stream);
    p->point_source_id_ = unpack<uint16_t>(in_stream);
    p->gps_time_ = unpack<double>(in_stream);
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

    for (uint32_t i = 0; i < eb_byte_size; i++)
    {
        p->extra_bytes_[i] = unpack<uint8_t>(in_stream);
    }
    return p;
}

void Point::Pack(std::ostream &out_stream, const Vector3 &scale, const Vector3 &offset) const
{
    // Point
    pack(RemoveScale<int32_t>(x_scaled_, scale.x, offset.x), out_stream);
    pack(RemoveScale<int32_t>(y_scaled_, scale.y, offset.y), out_stream);
    pack(RemoveScale<int32_t>(z_scaled_, scale.z, offset.z), out_stream);
    pack(intensity_, out_stream);
    pack(returns_, out_stream);
    pack(flags_, out_stream);
    pack(classification_, out_stream);
    pack(user_data_, out_stream);
    pack(scan_angle_, out_stream);
    pack(point_source_id_, out_stream);

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
    if (point_format_id < 6 || point_format_id > 8)
        throw std::runtime_error("Point format must be 6-8.");
    // Update flags
    has_rgb_ = FormatHasRgb(point_format_id);
    has_nir_ = FormatHasNir(point_format_id);
    point_record_length_ = PointBaseByteSize(point_format_id) + extra_bytes_.size();
    point_format_id_ = point_format_id;
}

std::string Point::ToString() const
{
    std::stringstream ss;
    ss << "Point: " << std::endl;
    ss << "\tX: " << x_scaled_ << ", Y: " << y_scaled_ << ", Z: " << z_scaled_ << std::endl;
    ss << "\tIntensity: " << intensity_ << std::endl;
    ss << "\tReturn Number: " << static_cast<short>(ReturnNumber())
       << ", Number of Returns: " << static_cast<short>(NumberOfReturns()) << std::endl;
    ss << "\tClassification Flags: Synthetic: " << Synthetic() << ", Key Point: " << KeyPoint()
       << ", Withheld: " << Withheld() << ", Overlap: " << Overlap() << std::endl;
    ss << "\tScannerChannel: " << static_cast<short>(ScannerChannel()) << std::endl;
    ss << "\tScan Direction: " << ScanDirectionFlag() << std::endl;
    ss << "\tEdge of Flight Line: " << EdgeOfFlightLineFlag() << std::endl;
    ss << "\tClasification: " << static_cast<short>(Classification()) << std::endl;
    ss << "\tUser Data: " << static_cast<short>(user_data_) << std::endl;
    ss << "\tScan Angle: " << scan_angle_ << std::endl;
    ss << "\tPoint Source ID: " << point_source_id_ << std::endl;
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
