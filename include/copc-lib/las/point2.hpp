#ifndef COPCLIB_LAS_POINT2_H_
#define COPCLIB_LAS_POINT2_H_

#include <vector>

namespace copc::las {

class Point2 {
public:

    Point2() = default;

    void Unpack(std::istream &in_stream, const uint16_t &point_format_id,const uint16_t &point_record_length);

    void Pack(std::ostream &out_stream) const;

    void PackAsFormat(std::ostream &out_stream, const uint8_t &point_format_id) const;

    static uint8_t BaseByteSize(const uint8_t &point_format_id);

    // Getters and Setters
    int32_t X() const { return x_; }
    void X(const int32_t &x) { x_ = x; }

    int32_t Y() const { return y_; }
    void Y(const int32_t &y) { y_ = y; }

    int32_t Z() const { return z_; }
    void Z(const int32_t &z) { z_ = z; }

    uint16_t Intensity() const { return intensity_; }
    void Intensity(const uint16_t &intensity) { intensity_ = intensity; }

    uint8_t ReturnsScanDirEofBitFields() const {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have ReturnsScanDirEofBitFields, use ReturnsBitFields instead.");
        else
            return returns_flags_eof_;
    }
    void ReturnsScanDirEofBitFields(const uint8_t &returns_bit_fields) {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have ReturnsScanDirEofBitFields, use ReturnsBitFields instead.");
        else
            returns_flags_eof_ = returns_bit_fields;
    }

    uint8_t ExtendedReturns() const {
        if (!extended_point_type_)
            throw std::runtime_error("Point10 does not have ReturnsBitFields, use ReturnsScanDirEofBitFields instead.");
        else
            return extended_returns_;
    }
    void ExtendedReturns(const uint8_t &extended_returns) {
        if (!extended_point_type_)
            throw std::runtime_error("Point10 does not have extended returns, use ReturnsScanDirEofBitFields instead.");
        else
            extended_returns_ = extended_returns;
    }

    uint8_t ReturnNumber() const { return extended_point_type_ ? extended_returns_ & 0xF : returns_flags_eof_ & 0x7; }
    void ReturnNumber(const uint8_t &return_number) {
        if (extended_point_type_)
            if (return_number > 15)
                throw std::runtime_error("Return Number must be <= 15");
            else
                extended_returns_ = return_number | (extended_returns_ & 0xF0);
        else if (return_number > 7)
            throw std::runtime_error("Return Number must be <= 7");
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0xF8) | return_number;
    }

    uint8_t NumberOfReturns() const {
        return extended_point_type_ ? extended_returns_ >> 4 : (returns_flags_eof_ >> 3) & 0x7;
    }
    void NumberOfReturns(const uint8_t &number_of_returns) {
        if (extended_point_type_)
            if (number_of_returns > 15)
                throw std::runtime_error("Number of Returns must be <= 15");
            else
                extended_returns_ = (number_of_returns << 4) | (extended_returns_ & 0xF);
        else {
            if (number_of_returns > 7)
                throw std::runtime_error("Number of Return must be <= 7");
            else
                returns_flags_eof_ = (returns_flags_eof_ & 0xC7) | (number_of_returns << 3);
        }
    }

    uint8_t FlagsBitFields() const {
        if (extended_point_type_)
            return extended_flags_;
        else
            throw std::runtime_error("Point10 does not have extended flags.");

    }
    void FlagsBitFields(const uint8_t &class_flags) {
        if (extended_point_type_)
            extended_flags_ = class_flags;
        else
            throw std::runtime_error("Point10 does not have FlagsBitFields.");
    }

    bool Synthetic() const { return extended_point_type_ ? extended_flags_ & 0x1 : (classification_ >> 5) & 0x1; }
    void Synthetic(const bool &synthetic) {
        extended_point_type_ ? extended_flags_ = (extended_flags_ & 0xFE) | synthetic : classification_ =
                                                                                            (classification_ & 0xDF)
                                                                                                | (synthetic << 5);
    }

    bool KeyPoint() const { return extended_point_type_ ? (extended_flags_ >> 1) & 0x1 : (classification_ >> 6) & 0x1; }
    void KeyPoint(const bool &key_point) {
        extended_point_type_ ? extended_flags_ = (extended_flags_ & 0xFD) | (key_point << 1) : classification_ =
                                                                                                   (classification_
                                                                                                       & 0xBF)
                                                                                                       | (key_point
                                                                                                           << 6);
    }

    bool Withheld() const { return extended_point_type_ ? (extended_flags_ >> 2) & 0x1 : (classification_ >> 7) & 0x1; }
    void Withheld(const bool &withheld) {
        extended_point_type_ ? extended_flags_ = (extended_flags_ & 0xFB) | (withheld << 2) : classification_ =
                                                                                                  (classification_
                                                                                                      & 0x7F)
                                                                                                      | (withheld << 7);
    }

    bool Overlap() const {
        if (extended_point_type_)
            (extended_flags_ >> 3) & 0x1;
        else
            throw std::runtime_error("Point10 does not have Overlap.");
    }
    void Overlap(const bool &overlap) {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0xF7) | (overlap << 3);
        else
            throw std::runtime_error("Point10 does not have Overlap.");
    }

    uint8_t ScannerChannel() const {
        if (extended_point_type_)
            (extended_flags_ >> 4) & 0x03;
        else
            throw std::runtime_error("Point10 does not have Scanner Channel.");

    }
    void ScannerChannel(const uint8_t &scanner_channel) {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0xCF) | (scanner_channel << 4);
        else
            throw std::runtime_error("Point10 does not have Scanner Channel.");
    }

    bool ScanDirFlag() const {
        return extended_point_type_ ? (extended_flags_ >> 6) & 0x1 : (returns_flags_eof_ >> 6) & 0x1;
    }
    void ScanDirFlag(const bool &scan_direction_flag) {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0xBF) | (scan_direction_flag << 6);
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0xBF) | (scan_direction_flag << 6);
    }

    bool EdgeOfFlightLineFlag() const {
        return extended_point_type_ ? extended_flags_ >> 7 : returns_flags_eof_ >> 7;
    }
    void EdgeOfFlightLineFlag(const bool &edge_of_flight_line) {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0x7F) | (edge_of_flight_line << 7);
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0x7F) | (edge_of_flight_line << 7);
    }

    uint8_t ClassificationBitFields() const {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Classification BitFields.");
        else
            return classification_;
    }

    void ClassificationBitFields(const uint8_t &classification_bitfields) {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Classification Bit Fields.");
        else
            classification_ = classification_bitfields;
    }

    uint8_t Classification() const { return extended_point_type_ ? extended_classification_ : classification_ & 0x1F; }
    void Classification(const uint8_t &classification) {
        if (extended_point_type_)
            classification_ = classification;
        else {
            if (classification > 31)
                throw std::runtime_error("Classification for Point10 must be <= 31");
            else
                classification_ = (classification_ & 0xE0) | classification;
        }
    }

    int8_t ScanAngleRank() const {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Scan Angle Rank.");
        else
            return scan_angle_rank_;
    }

    void ScanAngleRank(const int8_t &scan_angle_rank) {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Scan Angle Rank.");
        else
            if (scan_angle_rank < -90 || scan_angle_rank > 90)
                throw std::runtime_error("Scan Angle Rank must be between -90 and 90");
            else
                scan_angle_rank_ = scan_angle_rank;
    }

    int16_t ExtendedScanAngle() const {
        if (extended_point_type_)
            return extended_scan_angle_;
        else
            throw std::runtime_error("Point10 does not have Scan Angle.");
    }

    void ExtendedScanAngle(const int16_t &scan_angle) {
        if (extended_point_type_)
            extended_scan_angle_ = scan_angle;
        else
            throw std::runtime_error("Point10 does not have Scan Angle.");
    }

    float ScanAngle() const {
        return extended_point_type_ ? 0.006f * float(extended_scan_angle_) : float(scan_angle_rank_);
    }

    void ScanAngle(const float &scan_angle) {
        if (extended_point_type_)
            extended_scan_angle_ = static_cast<int16_t>(scan_angle / 0.006f);
        else {
            if (scan_angle < -90 || scan_angle > 90)
                throw std::runtime_error("Scan Angle Rank must be between -90 and 90");
            else
                scan_angle_rank_ = static_cast<int8_t>(scan_angle);
        }
    }

    uint8_t UserData() const { return user_data_; }
    void UserData(const uint8_t &user_data) { user_data_ = user_data; }

    uint16_t PointSourceId() const { return point_source_id_; }
    void PointSourceId(const uint16_t &point_source_id) { point_source_id_ = point_source_id; }

    void SetRgb(const uint16_t &r, const uint16_t &g, const uint16_t &b) {
        if (has_rgb_) {
            rgb_[0] = r;
            rgb_[1] = g;
            rgb_[2] = b;
        }
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t R() const {
        if (has_rgb_)
            return rgb_[0];
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void R(const uint16_t &r) {
        if (has_rgb_)
            rgb_[0] = r;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t G() const {
        if (has_rgb_)
            return rgb_[1];
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void G(const uint16_t g) {
        if (has_rgb_)
            rgb_[1] = g;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t B() const {
        if (has_rgb_)
            return rgb_[2];
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void B(const uint16_t &b) {
        if (has_rgb_)
            rgb_[2] = b;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    double GpsTime() const {
        if (has_gps_time_)
            return gps_time_;
        else
            throw std::runtime_error("This point format does not have GPS time.");
    }
    void GpsTime(const double &gps_time) {
        if (has_gps_time_)
            gps_time_ = gps_time;
        else
            throw std::runtime_error("This point format does not have GPS time.");
    }

    uint16_t Nir() const {
        if (has_nir_)
            return nir_;
        else
            throw std::runtime_error("This point format does not have NIR.");
    }
    void Nir(const uint16_t &nir) {
        if (has_nir_)
            nir_ = nir;
        else
            throw std::runtime_error("This point format does not have NIR.");
    }

    bool HasExtendedPoint() const { return extended_point_type_; }

    bool HasGpsTime() const { return has_gps_time_; }

    bool HasRgb() const { return has_rgb_; }

    bool HasNir() const { return has_nir_; }

    static bool FormatHasGps(const uint8_t &point_format_id) {
        switch(point_format_id) {
            case 0:
                return false;
            case 1:
                return true;
            case 2:
                return false;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                return true;
            default:throw std::runtime_error("Point format must be 0-10");
        }
    }

    static bool FormatHasRgb(const uint8_t &point_format_id) {
        switch(point_format_id) {
            case 0:
            case 1:
                return false;
            case 2:
            case 3:
                return true;
            case 4:
                return false;
            case 5:
                return true;
            case 6:
                return false;
            case 7:
            case 8:
                return true;
            case 9:
                return false;
            case 10:
                return true;
            default:throw std::runtime_error("Point format must be 0-10");
        }
    }

    static bool FormatHasNir(const uint8_t &point_format_id) {
        switch(point_format_id) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                return false;
            case 8:
                return true;
            case 9:
                return false;
            case 10:
                return true;
            default:throw std::runtime_error("Point format must be 0-10");
        }
    }

protected:

    int32_t x_;
    int32_t y_;
    int32_t z_;
    uint16_t intensity_;
    uint8_t returns_flags_eof_;
    uint8_t classification_;
    int8_t scan_angle_rank_;
    uint8_t user_data_;
    uint16_t point_source_id_;

    // LAS 1.4 only
    bool extended_point_type_;
    uint8_t extended_returns_;
    uint8_t extended_flags_;
    uint8_t extended_classification_;
    int16_t extended_scan_angle_;

    double gps_time_;
    uint16_t rgb_[3];
    uint16_t nir_;

    bool has_gps_time_;
    bool has_rgb_;
    bool has_nir_;

    std::vector<internal::ExtraByte> extra_bytes_;

    uint32_t point_byte_size_;
};

} // namespace copc::las
#endif // COPCLIB_LAS_POINT2_H_