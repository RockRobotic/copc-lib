#include <copc-lib/las/point2.hpp>

#include <copc-lib/las/internal/utils.hpp>

namespace copc::las {

void Point2::Unpack(std::istream &in_stream, const uint16_t &point_format_id, const uint16_t &point_record_length) {
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
        classification_ = internal::unpack<uint8_t>(in_stream);
        user_data_ = internal::unpack<uint8_t>(in_stream);
        extended_scan_angle_ = internal::unpack<int16_t>(in_stream);
    } else {
        returns_flags_eof_ = internal::unpack<uint8_t>(in_stream);
        extended_classification_ = internal::unpack<uint8_t>(in_stream);
        scan_angle_rank_ = internal::unpack<int8_t>(in_stream);
        user_data_ = internal::unpack<uint8_t>(in_stream);
    }
    point_source_id_ = internal::unpack<uint16_t>(in_stream);

    if (FormatHasGpsTime(point_format_id))
        gps_time_ = internal::unpack<double>(in_stream);
    if (FormatHasRgb(point_format_id)) {
        rgb_[0] = internal::unpack<uint16_t>(in_stream);
        rgb_[1] = internal::unpack<uint16_t>(in_stream);
        rgb_[2] = internal::unpack<uint16_t>(in_stream);
    }
    if (FormatHasNir(point_format_id)) {
        nir_ = internal::unpack<uint16_t>(in_stream);
    }

    point_byte_size_ = BaseByteSize(point_format_id);

    for (uint32_t i = 0; i < (point_record_length - point_byte_size_); i++) {
        extra_bytes_.emplace_back(internal::unpack<uint8_t>(in_stream));
    }
}

void Point2::Pack(std::ostream &out_stream) const {

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

uint8_t Point2::BaseByteSize(const uint8_t &point_format_id) {
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

void Point2::ToPointFormat(const uint8_t &point_format_id) {
    extended_point_type_ = point_format_id > 5;
    has_gps_time_ = FormatHasGpsTime(point_format_id);
    has_rgb_ = FormatHasRgb(point_format_id);
    has_nir_ = FormatHasNir(point_format_id);
}

//std::string Point2::ToString() const {
//    std::stringstream ss;
//    if (point_10_)
//        ss << point_10_->ToString();
//    else
//        ss << point_14_->ToString();
//    if (gps_time_)
//        ss << std::endl << gps_time_->ToString();
//    if (rgb_)
//        ss << std::endl << rgb_->ToString();
//    if (nir_)
//        ss << std::endl << nir_->ToString();
//    if (!extra_bytes_.empty())
//        ss << std::endl << "Extra Bytes: " << extra_bytes_.size();
//
//    return ss.str();
//}

//void Point2::PackAsFormat(std::ostream &out_stream, const uint8_t &point_format_id) const;


} // namespace copc::las