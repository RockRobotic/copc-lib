#ifndef COPCLIB_LAS_POINT_H_
#define COPCLIB_LAS_POINT_H_

#include <sstream>
#include <vector>

#include "header.hpp"
#include <copc-lib/las/header.hpp>
#include <copc-lib/las/utils.hpp>

namespace copc::las
{
class Point
{
  public:
    Point(const int8_t &point_format_id, const Vector3 &scale = Vector3::DefaultScale(),
          const Vector3 &offset = Vector3::DefaultOffset(), const uint16_t &num_extra_bytes = 0);
    Point(const LasHeader &header);
    Point(const Point &other);

    // Getters and Setters

#pragma region XYZ
    // XYZ Scaled
    double X() const { return ApplyScale(x_, scale_.x, offset_.x); }
    void X(const double &x) { x_ = RemoveScale<int32_t>(x, scale_.x, offset_.x); }

    double Y() const { return ApplyScale(y_, scale_.y, offset_.y); }
    void Y(const double &y) { y_ = RemoveScale<int32_t>(y, scale_.y, offset_.y); }

    double Z() const { return ApplyScale(z_, scale_.z, offset_.z); }
    void Z(const double &z) { z_ = RemoveScale<int32_t>(z, scale_.z, offset_.z); }

    // XYZ Unscaled
    int32_t UnscaledX() const { return x_; }
    void UnscaledX(const int32_t &x) { x_ = x; }

    int32_t UnscaledY() const { return y_; }
    void UnscaledY(const int32_t &y) { y_ = y; }

    int32_t UnscaledZ() const { return z_; }
    void UnscaledZ(const int32_t &z) { z_ = z; }
#pragma endregion XYZ

    uint16_t Intensity() const { return intensity_; }
    void Intensity(const uint16_t &intensity) { intensity_ = intensity; }

    uint8_t ReturnNumber() const { return extended_point_type_ ? extended_returns_ & 0xF : returns_flags_eof_ & 0x7; }
    void ReturnNumber(const uint8_t &return_number)
    {
        if (extended_point_type_)
        {
            if (return_number > 15)
                throw std::runtime_error("Return Number must be <= 15");
            else
                extended_returns_ = return_number | (extended_returns_ & 0xF0);
        }
        else
        {
            if (return_number > 7)
                throw std::runtime_error("Return Number must be <= 7");
            else
                returns_flags_eof_ = (returns_flags_eof_ & 0xF8) | return_number;
        }
    }

    uint8_t NumberOfReturns() const
    {
        return extended_point_type_ ? extended_returns_ >> 4 : (returns_flags_eof_ >> 3) & 0x7;
    }
    void NumberOfReturns(const uint8_t &number_of_returns)
    {
        if (extended_point_type_)
        {
            if (number_of_returns > 15)
                throw std::runtime_error("Number of Returns must be <= 15");
            else
                extended_returns_ = (number_of_returns << 4) | (extended_returns_ & 0xF);
        }
        else
        {
            if (number_of_returns > 7)
                throw std::runtime_error("Number of Return must be <= 7");
            else
                returns_flags_eof_ = (returns_flags_eof_ & 0xC7) | (number_of_returns << 3);
        }
    }

    uint8_t Classification() const { return extended_point_type_ ? extended_classification_ : classification_ & 0x1F; }
    void Classification(const uint8_t &classification)
    {
        if (extended_point_type_)
            extended_classification_ = classification;
        else
        {
            if (classification > 31)
                throw std::runtime_error(
                    "Classification for Point10 must be <= 31. To override this, use ClassificationBitFields.");
            else
                classification_ = (classification_ & 0xE0) | classification;
        }
    }

#pragma region ScanAngle
    int8_t ScanAngleRank() const
    {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Scan Angle Rank.");
        else
            return scan_angle_rank_;
    }

    void ScanAngleRank(const int8_t &scan_angle_rank)
    {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Scan Angle Rank.");
        else if (scan_angle_rank < -90 || scan_angle_rank > 90)
            throw std::runtime_error("Scan Angle Rank must be between -90 and 90");
        else
            scan_angle_rank_ = scan_angle_rank;
    }

    int16_t ExtendedScanAngle() const
    {
        if (extended_point_type_)
            return extended_scan_angle_;
        else
            throw std::runtime_error("Point10 does not have Scan Angle.");
    }

    void ExtendedScanAngle(const int16_t &scan_angle)
    {
        if (extended_point_type_)
        {
            if (scan_angle < -30000 || scan_angle > 30000)
                throw std::runtime_error("Scan Angle must be between -30000 and 30000");
            else
                extended_scan_angle_ = scan_angle;
        }
        else
            throw std::runtime_error("Point10 does not have Scan Angle.");
    }
#pragma endregion ScanAngle

    float ScanAngle() const
    {
        return extended_point_type_ ? 0.006f * float(extended_scan_angle_) : float(scan_angle_rank_);
    }

    void ScanAngle(const float &scan_angle)
    {
        if (extended_point_type_)
            extended_scan_angle_ = static_cast<int16_t>(scan_angle / 0.006f);
        else
        {
            if (scan_angle < -90 || scan_angle > 90)
                throw std::runtime_error("Scan Angle Rank must be between -90 and 90");
            else
                scan_angle_rank_ = static_cast<int8_t>(scan_angle);
        }
    }

    uint8_t UserData() const { return user_data_; }
    void UserData(const uint8_t &user_data) { user_data_ = user_data; }

    uint16_t PointSourceID() const { return point_source_id_; }
    void PointSourceID(const uint16_t &point_source_id) { point_source_id_ = point_source_id; }

#pragma region Flags

    bool Synthetic() const { return extended_point_type_ ? extended_flags_ & 0x1 : (classification_ >> 5) & 0x1; }
    void Synthetic(const bool &synthetic)
    {
        extended_point_type_ ? extended_flags_ = (extended_flags_ & 0xFE) | synthetic
                             : classification_ = (classification_ & 0xDF) | (synthetic << 5);
    }

    bool KeyPoint() const { return extended_point_type_ ? (extended_flags_ >> 1) & 0x1 : (classification_ >> 6) & 0x1; }
    void KeyPoint(const bool &key_point)
    {
        extended_point_type_ ? extended_flags_ = (extended_flags_ & 0xFD) | (key_point << 1)
                             : classification_ = (classification_ & 0xBF) | (key_point << 6);
    }

    bool Withheld() const { return extended_point_type_ ? (extended_flags_ >> 2) & 0x1 : (classification_ >> 7) & 0x1; }
    void Withheld(const bool &withheld)
    {
        extended_point_type_ ? extended_flags_ = (extended_flags_ & 0xFB) | (withheld << 2)
                             : classification_ = (classification_ & 0x7F) | (withheld << 7);
    }

    bool Overlap() const
    {
        if (extended_point_type_)
            return (extended_flags_ >> 3) & 0x1;
        else
            throw std::runtime_error("Point10 does not have Overlap.");
    }
    void Overlap(const bool &overlap)
    {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0xF7) | (overlap << 3);
        else
            throw std::runtime_error("Point10 does not have Overlap.");
    }

    uint8_t ScannerChannel() const
    {
        if (extended_point_type_)
            return (extended_flags_ >> 4) & 0x03;
        else
            throw std::runtime_error("Point10 does not have Scanner Channel.");
    }
    void ScannerChannel(const uint8_t &scanner_channel)
    {
        if (extended_point_type_)
            if (scanner_channel > 3)
                throw std::runtime_error("Scanner channel must be <= 3");
            else
                extended_flags_ = (extended_flags_ & 0xCF) | (scanner_channel << 4);
        else
            throw std::runtime_error("Point10 does not have Scanner Channel.");
    }

    bool ScanDirectionFlag() const
    {
        return extended_point_type_ ? (extended_flags_ >> 6) & 0x1 : (returns_flags_eof_ >> 6) & 0x1;
    }
    void ScanDirectionFlag(const bool &scan_direction_flag)
    {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0xBF) | (scan_direction_flag << 6);
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0xBF) | (scan_direction_flag << 6);
    }

    bool EdgeOfFlightLineFlag() const { return extended_point_type_ ? extended_flags_ >> 7 : returns_flags_eof_ >> 7; }
    void EdgeOfFlightLineFlag(const bool &edge_of_flight_line)
    {
        if (extended_point_type_)
            extended_flags_ = (extended_flags_ & 0x7F) | (edge_of_flight_line << 7);
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0x7F) | (edge_of_flight_line << 7);
    }
#pragma endregion Flags

#pragma region RGB
    void RGB(const std::vector<uint16_t> &rgb)
    {
        if (has_rgb_)
        {
            if (rgb.size() != 3)
                throw std::runtime_error("RGB vector must be of size 3.");
            rgb_[0] = rgb[0];
            rgb_[1] = rgb[1];
            rgb_[2] = rgb[2];
        }
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    void RGB(const uint16_t &red, const uint16_t &green, const uint16_t &blue)
    {
        if (has_rgb_)
        {
            rgb_[0] = red;
            rgb_[1] = green;
            rgb_[2] = blue;
        }
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t Red() const
    {
        if (has_rgb_)
            return rgb_[0];
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void Red(const uint16_t &red)
    {
        if (has_rgb_)
            rgb_[0] = red;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t Green() const
    {
        if (has_rgb_)
            return rgb_[1];
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void Green(const uint16_t &green)
    {
        if (has_rgb_)
            rgb_[1] = green;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t Blue() const
    {
        if (has_rgb_)
            return rgb_[2];
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void Blue(const uint16_t &blue)
    {
        if (has_rgb_)
            rgb_[2] = blue;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }
#pragma endregion RGB

    double GPSTime() const
    {
        if (has_gps_time_)
            return gps_time_;
        else
            throw std::runtime_error("This point format does not have GPS time.");
    }
    void GPSTime(const double &gps_time)
    {
        if (has_gps_time_)
            gps_time_ = gps_time;
        else
            throw std::runtime_error("This point format does not have GPS time.");
    }

    uint16_t NIR() const
    {
        if (has_nir_)
            return nir_;
        else
            throw std::runtime_error("This point format does not have NIR.");
    }
    void NIR(const uint16_t &nir)
    {
        if (has_nir_)
            nir_ = nir;
        else
            throw std::runtime_error("This point format does not have NIR.");
    }

    std::vector<uint8_t> ExtraBytes() const { return extra_bytes_; }
    void ExtraBytes(const std::vector<uint8_t> &in)
    {
        if (in.size() != NumExtraBytes())
            throw std::runtime_error("Number of input bytes " + std::to_string(in.size()) +
                                     " does not match number of point bytes " + std::to_string(NumExtraBytes()));
        extra_bytes_ = in;
    }

#pragma region BitFields
    uint8_t ReturnsScanDirEofBitFields() const
    {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have ReturnsScanDirEofBitFields, use ExtendedReturnsBitFields "
                                     "or ExtendedFlagsBitFields instead.");
        else
            return returns_flags_eof_;
    }
    void ReturnsScanDirEofBitFields(const uint8_t &returns_bit_fields)
    {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have ReturnsScanDirEofBitFields, use ExtendedReturnsBitFields "
                                     "or ExtendedFlagsBitFields instead.");
        else
            returns_flags_eof_ = returns_bit_fields;
    }

    uint8_t ExtendedReturnsBitFields() const
    {
        if (!extended_point_type_)
            throw std::runtime_error("Point10 does not have ReturnsBitFields, use ReturnsScanDirEofBitFields instead.");
        else
            return extended_returns_;
    }
    void ExtendedReturnsBitFields(const uint8_t &extended_returns)
    {
        if (!extended_point_type_)
            throw std::runtime_error("Point10 does not have ReturnsBitFields, use ReturnsScanDirEofBitFields instead.");
        else
            extended_returns_ = extended_returns;
    }

    uint8_t ExtendedFlagsBitFields() const
    {
        if (extended_point_type_)
            return extended_flags_;
        else
            throw std::runtime_error("Point10 does not have extended flags, use ReturnsScanDirEofBitFields instead.");
    }
    void ExtendedFlagsBitFields(const uint8_t &class_flags)
    {
        if (extended_point_type_)
            extended_flags_ = class_flags;
        else
            throw std::runtime_error("Point10 does not have FlagsBitFields, use ReturnsScanDirEofBitFields instead.");
    }

    uint8_t ClassificationBitFields() const
    {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Classification BitFields.");
        else
            return classification_;
    }

    void ClassificationBitFields(const uint8_t &classification_bitfields)
    {
        if (extended_point_type_)
            throw std::runtime_error("Point14 does not have Classification Bit Fields.");
        else
            classification_ = classification_bitfields;
    }
#pragma endregion BitFields

    bool HasExtendedPoint() const { return extended_point_type_; }
    bool HasGPSTime() const { return has_gps_time_; }
    bool HasRGB() const { return has_rgb_; }
    bool HasNIR() const { return has_nir_; }

    uint32_t PointRecordLength() const { return point_record_length_; }
    int8_t PointFormatID() const { return point_format_id_; }
    uint16_t NumExtraBytes() const;

    Vector3 Scale() const { return scale_; }
    Vector3 Offset() const { return offset_; }

    bool operator==(const Point &other) const;
    bool operator!=(const Point &other) const { return !(*this == other); };
    std::string ToString() const;

    static std::shared_ptr<Point> Unpack(std::istream &in_stream, const int8_t &point_format_id, const Vector3 &scale,
                                         const Vector3 &offset, const uint16_t &num_extra_bytes);
    void Pack(std::ostream &out_stream) const;
    void ToPointFormat(const int8_t &point_format_id);

  protected:
    int32_t x_{};
    int32_t y_{};
    int32_t z_{};
    uint16_t intensity_{};
    uint8_t returns_flags_eof_{};
    uint8_t classification_{};
    int8_t scan_angle_rank_{};
    uint8_t user_data_{};
    uint16_t point_source_id_{};

    // LAS 1.4 only
    bool extended_point_type_{false};
    uint8_t extended_returns_{};
    uint8_t extended_flags_{};
    uint8_t extended_classification_{};
    int16_t extended_scan_angle_{};

    double gps_time_{};
    uint16_t rgb_[3]{};
    uint16_t nir_{};

    bool has_gps_time_{false};
    bool has_rgb_{false};
    bool has_nir_{false};

    std::vector<uint8_t> extra_bytes_;

  private:
    uint32_t point_record_length_;
    int8_t point_format_id_;
    Vector3 scale_;
    Vector3 offset_;
};

} // namespace copc::las
#endif // COPCLIB_LAS_POINT_H_
