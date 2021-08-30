#ifndef COPCLIB_LAS_INTERNAL_POINT_14_H_
#define COPCLIB_LAS_INTERNAL_POINT_14_H_

#include <sstream>

#include "utils.hpp"

namespace copc::las::internal
{
class Point14
{

  public:
    Point14()
        : x_(0), y_(0), z_(0), intensity_(0), returns_(0), flags_(0), classification_(0), user_data_(0), scan_angle_(0),
          point_source_id_(0){};

    Point14(std::istream &in_stream) { Unpack(in_stream); }

    int32_t X() const { return x_; }
    void X(const int32_t &x) { x_ = x; }

    int32_t Y() const { return y_; }
    void Y(const int32_t &y) { y_ = y; }

    int32_t Z() const { return z_; }
    void Z(const int32_t &z) { z_ = z; }

    uint16_t Intensity() const { return intensity_; }
    void Intensity(const uint16_t &intensity) { intensity_ = intensity; }

    uint8_t ReturnsBitFields() const { return returns_; }
    void ReturnsBitFields(const uint8_t &returns) { returns_ = returns; }

    uint8_t ReturnNumber() const { return returns_ & 0xF; }
    void ReturnNumber(const uint8_t &return_number)
    {
        if (return_number > 15)
            throw std::runtime_error("Return Number must be <= 15");
        else
            returns_ = return_number | (returns_ & 0xF0);
    }

    uint8_t NumberOfReturns() const { return returns_ >> 4; }
    void NumberOfReturns(const int &number_of_returns)
    {
        if (number_of_returns > 15)
            throw std::runtime_error("Number of Returns must be <= 15");
        else
            returns_ = (number_of_returns << 4) | (returns_ & 0xF);
    }

    uint8_t FlagsBitFields() const { return flags_; }
    void FlagsBitFields(const uint8_t &flags) { flags_ = flags; }

    bool Synthetic() const { return (flags_ >> 4) & 0x1; }
    void Synthetic(const bool &synthetic) { flags_ = (flags_ & 0xEF) | (synthetic << 4); }

    bool KeyPoint() const { return (flags_ >> 5) & 0x1; }
    void KeyPoint(const bool &keypoint) { flags_ = (flags_ & 0xDF) | (keypoint << 5); }

    bool Withheld() const { return (flags_ >> 6) & 0x1; }
    void Withheld(const bool &withheld) { flags_ = (flags_ & 0xBF) | (withheld << 6); }

    bool Overlap() const { return (flags_ >> 7) & 0x1; }
    void Overlap(const bool &overlap) { flags_ = (flags_ & 0x7F) | (overlap << 7); }

    uint8_t ScannerChannel() const { return (flags_ >> 4) & 0x03; }
    void ScannerChannel(const uint8_t &scanner_channel)
    {
        if (scanner_channel > 3)
            throw std::runtime_error("Scanner channel must be <= 3");
        else
            flags_ = (scanner_channel << 4) | (flags_ & 0xCF);
    }

    bool ScanDirectionFlag() const { return ((flags_ >> 6) & 1); }
    void ScanDirectionFlag(const bool &scan_dir_flag) { flags_ = (scan_dir_flag << 6) | (flags_ & 0xBF); }

    bool EdgeOfFlightLineFlag() const { return flags_ >> 7; }
    void EdgeOfFlightLineFlag(const bool &eof_flag) { flags_ = (eof_flag << 7) | (flags_ & 0x7F); }

    uint8_t Classification() const { return classification_; }
    void Classification(const uint8_t &classification) { classification_ = classification; }

    uint8_t UserData() const { return user_data_; }
    void UserData(const uint8_t &user_data) { user_data_ = user_data; }

    int16_t ScanAngle() const { return scan_angle_; }
    void ScanAngle(const int16_t &scan_angle)
    {
        if (scan_angle < -30000 || scan_angle > 30000)
            throw std::runtime_error("Scan Angle must be between -30000 and 30000");
        else
            scan_angle_ = scan_angle;
    }

    uint16_t PointSourceId() const { return point_source_id_; }
    void PointSourceId(const uint16_t &point_source_ID) { point_source_id_ = point_source_ID; }

    void Unpack(std::istream &in_stream)
    {
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
    void Pack(std::ostream &out_stream) const
    {
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

    std::string ToString() const
    {
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

  private:
    int32_t x_;
    int32_t y_;
    int32_t z_;
    uint16_t intensity_;
    uint8_t returns_;
    uint8_t flags_;
    uint8_t classification_;
    uint8_t user_data_;
    int16_t scan_angle_;
    uint16_t point_source_id_;
};
} // namespace copc::las::internal

#endif // COPCLIB_LAS_INTERNAL_POINT_14_H_
