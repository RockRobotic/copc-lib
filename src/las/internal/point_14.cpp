#include <copc-lib/las/internal/point_14.hpp>

#include <sstream>

#include <copc-lib/las/internal/point_10.hpp>

namespace copc::las::internal {

Point14::Point14(int32_t x,
                 int32_t y,
                 int32_t z,
                 uint16_t intensity,
                 uint8_t return_number,
                 uint8_t number_of_returns,
                 bool synthetic,
                 bool keypoint,
                 bool withheld,
                 bool overlap,
                 uint8_t scanner_channel,
                 bool scan_direction_flag,
                 bool edge_of_flight_line_flag,
                 uint8_t classification,
                 uint8_t user_data,
                 int16_t scan_angle,
                 uint16_t point_source_id)
    : x_(x),
      y_(y),
      z_(z),
      intensity_(intensity),
      classification_(classification),
      user_data_(user_data),
      scan_angle_(scan_angle),
      point_source_id_(point_source_id) {
    ReturnNumber(return_number);
    NumberOfReturns(number_of_returns);
    Synthetic(synthetic);
    KeyPoint(keypoint);
    Withheld(withheld);
    Overlap(overlap);
    ScannerChannel(scanner_channel);
    ScanDirectionFlag(scan_direction_flag);
    EdgeOfFlightLineFlag(edge_of_flight_line_flag);
}

void Point14::Unpack(std::istream &in_stream) {
    X(internal::unpack<int32_t>(in_stream));
    Y(internal::unpack<int32_t>(in_stream));
    Z(internal::unpack<int32_t>(in_stream));
    Intensity(internal::unpack<uint16_t>(in_stream));
    ReturnsBitFields(internal::unpack<uint8_t>(in_stream));
    FlagsBitFields(internal::unpack<uint8_t>(in_stream));
    Classification(internal::unpack<uint8_t>(in_stream));
    UserData(internal::unpack<uint8_t>(in_stream));
    ScanAngle(internal::unpack<int16_t>(in_stream));
    PointSourceId(internal::unpack<uint16_t>(in_stream));
}

void Point14::Pack(std::ostream &out_stream) const {
    internal::pack(x_, out_stream);
    internal::pack(y_, out_stream);
    internal::pack(z_, out_stream);
    internal::pack(intensity_, out_stream);
    internal::pack(returns_, out_stream);
    internal::pack(flags_, out_stream);
    internal::pack(classification_, out_stream);
    internal::pack(user_data_, out_stream);
    internal::pack(scan_angle_, out_stream);
    internal::pack(point_source_id_, out_stream);
}

std::string Point14::ToString() const {
    std::stringstream ss;
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
    ss << "\tScan Angle: " << scan_angle_ << std::endl;
    ss << "\tPoint Source ID: " << point_source_id_;

    return ss.str();
}

Point10 Point14::ToPoint10() const {
    return Point10(x_,
                   y_,
                   z_,
                   intensity_,
                   std::min(int(ReturnNumber()), 7),
                   std::min(int(NumberOfReturns()), 7),
                   ScanDirectionFlag(),
                   EdgeOfFlightLineFlag(),
                   std::min(int(classification_), 31),
                   Synthetic(),
                   KeyPoint(),
                   Withheld(),
                   user_data_,
                   static_cast<int8_t>(static_cast<double>(std::max(-15000,std::min(int(15000),int(scan_angle_))))*0.006),
                   point_source_id_);
}

} // namespace copc::las::internal
