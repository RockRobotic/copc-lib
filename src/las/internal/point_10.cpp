#include <copc-lib/las/internal/point_10.hpp>

#include <sstream>

#include <copc-lib/las/internal/point_14.hpp>

namespace copc::las::internal {

Point10::Point10(int32_t x,
                 int32_t y,
                 int32_t z,
                 uint16_t intensity,
                 uint8_t return_number,
                 uint8_t number_of_returns,
                 bool scan_direction_flag,
                 bool edge_of_flight_line_flag,
                 uint8_t classification,
                 bool synthetic,
                 bool keypoint,
                 bool withheld,
                 uint8_t user_data,
                 int8_t scan_angle_rank,
                 uint16_t point_source_id)
    : x_(x),
      y_(y),
      z_(z),
      intensity_(intensity),
      user_data_(user_data),
      scan_angle_rank_(scan_angle_rank),
      point_source_id_(point_source_id) {
    ReturnNumber(return_number);
    NumberOfReturns(number_of_returns);
    ScanDirectionFlag(scan_direction_flag);
    EdgeOfFlightLineFlag(edge_of_flight_line_flag);
    Classification(classification);
    Synthetic(synthetic);
    KeyPoint(keypoint);
    Withheld(withheld);
}

void Point10::Unpack(std::istream &in_stream) {
    x_ = internal::unpack<int32_t>(in_stream);
    y_ = internal::unpack<int32_t>(in_stream);
    z_ = internal::unpack<int32_t>(in_stream);
    intensity_ = internal::unpack<uint16_t>(in_stream);
    returns_flags_eof_ = internal::unpack<uint8_t>(in_stream);
    classification_ = internal::unpack<uint8_t>(in_stream);
    scan_angle_rank_ = internal::unpack<int8_t>(in_stream);
    user_data_ = internal::unpack<uint8_t>(in_stream);
    point_source_id_ = internal::unpack<uint16_t>(in_stream);
}

void Point10::Pack(std::ostream &out_stream) const {
    internal::pack(x_, out_stream);
    internal::pack(y_, out_stream);
    internal::pack(z_, out_stream);
    internal::pack(intensity_, out_stream);
    internal::pack(returns_flags_eof_, out_stream);
    internal::pack(classification_, out_stream);
    internal::pack(scan_angle_rank_, out_stream);
    internal::pack(user_data_, out_stream);
    internal::pack(point_source_id_, out_stream);
}

std::string Point10::ToString() const {
    std::stringstream ss;
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

    return ss.str();
}

Point14 Point10::ToPoint14() const {
    return Point14(x_,
                   y_,
                   z_,
                   intensity_,
                   ReturnNumber(),
                   NumberOfReturns(),
                   Synthetic(),
                   KeyPoint(),
                   Withheld(),
                   false,
                   0,
                   ScanDirectionFlag(),
                   EdgeOfFlightLineFlag(),
                   Classification(),
                   user_data_,
                   static_cast<int16_t>(static_cast<double>(scan_angle_rank_)/0.006),
                   point_source_id_);
}

} // namespace copc::las::internal

