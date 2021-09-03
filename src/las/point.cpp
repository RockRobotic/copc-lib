#include <copc-lib/las/point.hpp>

#include <copc-lib/las/internal/utils.hpp>

namespace copc::las {

void Point::Unpack(std::istream &in_stream, const uint16_t &point_format_id, const uint16_t &point_record_length) {
    if (point_format_id > 10)
        throw std::runtime_error("Point format must be 0-10");

    if (point_format_id > 5)
        extended_point_type_ = true;

    x_ = internal::unpack<int32_t>(in_stream);
    y_ = internal::unpack<int32_t>(in_stream);
    z_ = internal::unpack<int32_t>(in_stream);
    intensity_ = internal::unpack<uint16_t>(in_stream);
    if (extended_point_type_) {
        extended_returns_ = internal::unpack<uint8_t>(in_stream);
        extended_flags_ = internal::unpack<uint8_t>(in_stream);
        extended_classification_ = internal::unpack<uint8_t>(in_stream);
        user_data_ = internal::unpack<uint8_t>(in_stream);
        extended_scan_angle_ = internal::unpack<int16_t>(in_stream);
    } else {
        returns_flags_eof_ = internal::unpack<uint8_t>(in_stream);
        classification_ = internal::unpack<uint8_t>(in_stream);
        scan_angle_rank_ = internal::unpack<int8_t>(in_stream);
        user_data_ = internal::unpack<uint8_t>(in_stream);
    }
    point_source_id_ = internal::unpack<uint16_t>(in_stream);

    if (FormatHasGpsTime(point_format_id)) {
        gps_time_ = internal::unpack<double>(in_stream);
        has_gps_time_ = true;
    }
    if (FormatHasRgb(point_format_id)) {
        rgb_[0] = internal::unpack<uint16_t>(in_stream);
        rgb_[1] = internal::unpack<uint16_t>(in_stream);
        rgb_[2] = internal::unpack<uint16_t>(in_stream);
        has_rgb_ = true;
    }
    if (FormatHasNir(point_format_id)) {
        nir_ = internal::unpack<uint16_t>(in_stream);
        has_nir_ = true;
    }

    point_byte_size_ = BaseByteSize(point_format_id);

    for (uint32_t i = 0; i < (point_record_length - point_byte_size_); i++) {
        extra_bytes_.emplace_back(internal::unpack<uint8_t>(in_stream));
    }
}

void Point::Pack(std::ostream &out_stream) const {

    // Point
    internal::pack(x_, out_stream);
    internal::pack(y_, out_stream);
    internal::pack(z_, out_stream);
    internal::pack(intensity_, out_stream);
    if (extended_point_type_) {
        internal::pack(extended_returns_, out_stream);
        internal::pack(extended_flags_, out_stream);
        internal::pack(classification_, out_stream);
        internal::pack(user_data_, out_stream);
        internal::pack(extended_scan_angle_, out_stream);
    } else {
        internal::pack(returns_flags_eof_, out_stream);
        internal::pack(classification_, out_stream);
        internal::pack(scan_angle_rank_, out_stream);
        internal::pack(user_data_, out_stream);
    }
    internal::pack(point_source_id_, out_stream);

    if (has_gps_time_)
        internal::pack(gps_time_, out_stream);
    if (has_rgb_) {
        internal::pack(rgb_[0], out_stream);
        internal::pack(rgb_[1], out_stream);
        internal::pack(rgb_[2], out_stream);
    }
    if (has_nir_)
        internal::pack(nir_, out_stream);

    for (auto eb: extra_bytes_)
        internal::pack(eb, out_stream);
}

uint8_t Point::BaseByteSize(const uint8_t &point_format_id) {
    switch (point_format_id) {
        case 0:return 20;
        case 1:return 28;
        case 2:return 26;
        case 3:return 34;
        case 4:return 28;
        case 5:return 34;
        case 6:return 30;
        case 7:return 36;
        case 8:return 38;
        case 9:return 30;
        case 10:return 38;
        default:throw std::runtime_error("Point format must be 0-10");;
    }
}

void Point::ToPointFormat(const uint8_t &point_format_id) {

    if (extended_point_type_ && point_format_id < 6) {
        // Convert points values from 1.4 to 1.0
        // Returns and flags
        returns_flags_eof_ =
            (EdgeOfFlightLineFlag() << 7) | (ScanDirectionFlag() << 6) | (std::min(int(NumberOfReturns()), 7) << 3)
                | std::min(int(ReturnNumber()), 7);
        // Classification
        classification_ =
            (Withheld() << 7) | (KeyPoint() << 6) | (Synthetic() << 5) | std::min(int(Classification()), 31);

        // Scan angle
        scan_angle_rank_ = static_cast<int8_t>(std::max(-90, std::min(90, int(float(ExtendedScanAngle()) * 0.006f))));

    } else if (!extended_point_type_ && point_format_id > 5) {
        // Convert point values from 1.0 to 1.4
        // Returns
        extended_returns_ = (NumberOfReturns() << 4) | ReturnNumber();
        // Flags
        extended_flags_ =
            (EdgeOfFlightLineFlag() << 7) | (ScanDirectionFlag() << 6) | (Withheld() << 2) | (KeyPoint() << 1)
                | Synthetic();
        //Classification
        extended_classification_ = Classification();
        // Scan angle
        extended_scan_angle_ = static_cast<int16_t>(float(ScanAngleRank()) / 0.006f);
    }

    // Update flags
    extended_point_type_ = point_format_id > 5;
    has_gps_time_ = FormatHasGpsTime(point_format_id);
    has_rgb_ = FormatHasRgb(point_format_id);
    has_nir_ = FormatHasNir(point_format_id);
}

std::string Point::ToString() const {
    std::stringstream ss;
    if (extended_point_type_) {
        ss << "Point14: " << std::endl;
        ss << "\tX: " << x_ << ", Y: " << y_ << ", Z: " << z_ << std::endl;
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
        ss << "\tScan Angle: " << extended_scan_angle_ << std::endl;
        ss << "\tPoint Source ID: " << point_source_id_;
    } else {
        ss << "Point10: " << std::endl;
        ss << "\tX: " << x_ << ", Y: " << y_ << ", Z: " << z_ << std::endl;
        ss << "\tIntensity: " << intensity_ << std::endl;
        ss << "\tReturn Number: " << static_cast<short>(ReturnNumber())
           << ", Number of Returns: " << static_cast<short>(NumberOfReturns()) << std::endl;
        ss << "\tScan Direction: " << ScanDirectionFlag() << std::endl;
        ss << "\tEdge of Flight Line: " << EdgeOfFlightLineFlag() << std::endl;
        ss << "\tClasification: " << static_cast<short>(Classification()) << std::endl;
        ss << "\tClassification Flags: Synthetic: " << Synthetic() << ", Key Point: " << KeyPoint()
           << ", Withheld: " << Withheld() << std::endl;
        ss << "\tScan Angle Rank: " << static_cast<short>(scan_angle_rank_) << std::endl;
        ss << "\tUser Data: " << static_cast<short>(user_data_) << std::endl;
        ss << "\tPoint Source ID: " << point_source_id_;
    }
    if (has_gps_time_)
        ss << std::endl << "GPS Time: " << gps_time_;
    if (has_rgb_)
        ss << std::endl << "RGB: [" << rgb_[0] << "," << rgb_[1] << "," << rgb_[2] << "]";
    if (has_nir_)
        ss << std::endl << "NIR: " << nir_;
    if (!extra_bytes_.empty())
        ss << std::endl << "Extra Bytes: " << extra_bytes_.size();

    return ss.str();
}

//void Point::PackAsFormat(std::ostream &out_stream, const uint8_t &point_format_id) const;


} // namespace copc::las