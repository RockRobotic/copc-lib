#ifndef COPCLIB_LAS_POINT_H_
#define COPCLIB_LAS_POINT_H_

#include <sstream>
#include <vector>

#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/las/header.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc::las
{
class Point
{
  public:
    Point(const int8_t &point_format_id, const uint16_t &eb_byte_size = 0);
    Point(const LasHeader &header);
    Point(const Point &other);

    // Getters and Setters

#pragma region XYZ
    // XYZ Scaled
    double X() const { return x_scaled_; }
    void X(const double &x) { x_scaled_ = x; }

    double Y() const { return y_scaled_; }
    void Y(const double &y) { y_scaled_ = y; }

    double Z() const { return z_scaled_; }
    void Z(const double &z) { z_scaled_ = z; }

#pragma endregion XYZ

    uint16_t Intensity() const { return intensity_; }
    void Intensity(const uint16_t &intensity) { intensity_ = intensity; }

    uint8_t ReturnNumber() const { return returns_ & 0xF; }
    void ReturnNumber(const uint8_t &return_number)
    {
        if (return_number > 15)
            throw std::runtime_error("Return Number must be <= 15");
        returns_ = return_number | (returns_ & 0xF0);
    }

    uint8_t NumberOfReturns() const { return returns_ >> 4; }
    void NumberOfReturns(const uint8_t &number_of_returns)
    {
        if (number_of_returns > 15)
            throw std::runtime_error("Number of Returns must be <= 15");
        returns_ = (number_of_returns << 4) | (returns_ & 0xF);
    }

    uint8_t Classification() const { return classification_; }
    void Classification(const uint8_t &classification) { classification_ = classification; }

    int16_t ScanAngle() const { return scan_angle_; }

    void ScanAngle(const int16_t &scan_angle)
    {
        // TODO[Leo]: Add a verbose option to copclib to output warnings instead of throwing errors.
        //         if (scan_angle < -30000 || scan_angle > 30000)
        //            std::cout << "Warning Scan Angle must be between -30000 and 30000";
        scan_angle_ = scan_angle;
    }

    float ScanAngleDegrees() const { return 0.006f * float(scan_angle_); }

    void ScanAngleDegrees(const float &scan_angle) { scan_angle_ = static_cast<int16_t>(scan_angle / 0.006f); }

    uint8_t UserData() const { return user_data_; }
    void UserData(const uint8_t &user_data) { user_data_ = user_data; }

    uint16_t PointSourceId() const { return point_source_id_; }
    void PointSourceId(const uint16_t &point_source_id) { point_source_id_ = point_source_id; }

#pragma region Flags

    bool Synthetic() const { return flags_ & 0x1; }
    void Synthetic(const bool &synthetic) { flags_ = (flags_ & 0xFE) | synthetic; }

    bool KeyPoint() const { return (flags_ >> 1) & 0x1; }
    void KeyPoint(const bool &key_point) { flags_ = (flags_ & 0xFD) | (key_point << 1); }

    bool Withheld() const { return (flags_ >> 2) & 0x1; }
    void Withheld(const bool &withheld) { flags_ = (flags_ & 0xFB) | (withheld << 2); }

    bool Overlap() const { return (flags_ >> 3) & 0x1; }
    void Overlap(const bool &overlap) { flags_ = (flags_ & 0xF7) | (overlap << 3); }

    uint8_t ScannerChannel() const { return (flags_ >> 4) & 0x03; }
    void ScannerChannel(const uint8_t &scanner_channel)
    {
        if (scanner_channel > 3)
            throw std::runtime_error("Scanner channel must be <= 3");
        flags_ = (flags_ & 0xCF) | (scanner_channel << 4);
    }

    bool ScanDirectionFlag() const { return (flags_ >> 6) & 0x1; }
    void ScanDirectionFlag(const bool &scan_direction_flag) { flags_ = (flags_ & 0xBF) | (scan_direction_flag << 6); }

    bool EdgeOfFlightLineFlag() const { return flags_ >> 7; }
    void EdgeOfFlightLineFlag(const bool &edge_of_flight_line)
    {
        flags_ = (flags_ & 0x7F) | (edge_of_flight_line << 7);
    }
#pragma endregion Flags

#pragma region RGB
    void Rgb(const std::vector<uint16_t> &rgb)
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB.");

        if (rgb.size() != 3)
            throw std::runtime_error("RGB vector must be of size 3.");
        rgb_[0] = rgb[0];
        rgb_[1] = rgb[1];
        rgb_[2] = rgb[2];
    }

    void Rgb(const uint16_t &red, const uint16_t &green, const uint16_t &blue)
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB.");
        rgb_[0] = red;
        rgb_[1] = green;
        rgb_[2] = blue;
    }

    uint16_t Red() const
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB");
        return rgb_[0];
    }
    void Red(const uint16_t &red)
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB.");
        rgb_[0] = red;
    }

    uint16_t Green() const
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB");
        return rgb_[1];
    }
    void Green(const uint16_t &green)
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB.");
        rgb_[1] = green;
    }

    uint16_t Blue() const
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB");
        return rgb_[2];
    }
    void Blue(const uint16_t &blue)
    {
        if (!has_rgb_)
            throw std::runtime_error("This point format does not have RGB.");
        rgb_[2] = blue;
    }
#pragma endregion RGB

    double GPSTime() const { return gps_time_; }
    void GPSTime(const double &gps_time) { gps_time_ = gps_time; }

    uint16_t Nir() const
    {
        if (!has_nir_)
            throw std::runtime_error("This point format does not have NIR.");
        return nir_;
    }
    void Nir(const uint16_t &nir)
    {
        if (!has_nir_)
            throw std::runtime_error("This point format does not have NIR.");
        nir_ = nir;
    }

    std::vector<uint8_t> ExtraBytes() const { return extra_bytes_; }
    void ExtraBytes(const std::vector<uint8_t> &in)
    {
        if (in.size() != EbByteSize())
            throw std::runtime_error("Number of input bytes " + std::to_string(in.size()) +
                                     " does not match number of point bytes " + std::to_string(EbByteSize()));
        extra_bytes_ = in;
    }

#pragma region BitFields

    uint8_t ReturnsBitField() const { return returns_; }
    void ReturnsBitField(const uint8_t &extended_returns) { returns_ = extended_returns; }

    uint8_t FlagsBitField() const { return flags_; }
    void FlagsBitField(const uint8_t &class_flags) { flags_ = class_flags; }
#pragma endregion BitFields

    bool HasRgb() const { return has_rgb_; }
    bool HasNir() const { return has_nir_; }

    uint32_t PointRecordLength() const { return point_record_length_; }
    int8_t PointFormatId() const { return point_format_id_; }
    uint16_t EbByteSize() const;

    bool operator==(const Point &other) const;
    bool operator!=(const Point &other) const { return !(*this == other); };

    bool Within(const Box &box) const;

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, Point const &value)
    {
        os << value.ToString();
        return os;
    }

    static std::shared_ptr<Point> Unpack(std::istream &in_stream, const int8_t &point_format_id, const Vector3 &scale,
                                         const Vector3 &offset, const uint16_t &eb_byte_size);
    void Pack(std::ostream &out_stream, const Vector3 &scale, const Vector3 &offset) const;
    void ToPointFormat(const int8_t &point_format_id);

  protected:
    double x_scaled_{};
    double y_scaled_{};
    double z_scaled_{};
    uint16_t intensity_{};
    uint8_t returns_{};
    uint8_t flags_{};
    uint8_t classification_{};
    int16_t scan_angle_{};
    uint8_t user_data_{};
    uint16_t point_source_id_{};

    double gps_time_{};
    uint16_t rgb_[3]{};
    uint16_t nir_{};

    bool has_rgb_{false};
    bool has_nir_{false};

    std::vector<uint8_t> extra_bytes_;

  private:
    uint32_t point_record_length_;
    int8_t point_format_id_;
};

} // namespace copc::las
#endif // COPCLIB_LAS_POINT_H_
