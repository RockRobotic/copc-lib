#ifndef COPCLIB_LAS_INTERNAL_POINT_14_H_
#define COPCLIB_LAS_INTERNAL_POINT_14_H_

#include <sstream>

#include <copc-lib/las/internal/utils.hpp>

namespace copc::las::internal {

class Point10;
class Point14 {

public:
    Point14()
        : x_(0), y_(0), z_(0), intensity_(0), returns_(0), flags_(0), classification_(0), user_data_(0), scan_angle_(0),
          point_source_id_(0) {};

    Point14(int32_t x,
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
            uint16_t point_source_id);

    Point14(std::istream &in_stream) { Unpack(in_stream); }

    void Unpack(std::istream &in_stream);

    void Pack(std::ostream &out_stream) const;

    std::string ToString() const;

    Point10 ToPoint10() const;

    // Getters and Setters
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
    void ReturnNumber(const uint8_t &return_number) {
        if (return_number > 15)
            throw std::runtime_error("Return Number must be <= 15");
        else
            returns_ = return_number | (returns_ & 0xF0);
    }

    uint8_t NumberOfReturns() const { return returns_ >> 4; }
    void NumberOfReturns(const int &number_of_returns) {
        if (number_of_returns > 15)
            throw std::runtime_error("Number of Returns must be <= 15");
        else
            returns_ = (number_of_returns << 4) | (returns_ & 0xF);
    }

    uint8_t FlagsBitFields() const { return flags_; }
    void FlagsBitFields(const uint8_t &flags) { flags_ = flags; }

    bool Synthetic() const { return flags_ & 0x1; }
    void Synthetic(const bool &synthetic) { flags_ = (flags_ & 0xFE) | synthetic; }

    bool KeyPoint() const { return (flags_ >> 1) & 0x1; }
    void KeyPoint(const bool &keypoint) { flags_ = (flags_ & 0xFD) | (keypoint << 1); }

    bool Withheld() const { return (flags_ >> 2) & 0x1; }
    void Withheld(const bool &withheld) { flags_ = (flags_ & 0xFB) | (withheld << 2); }

    bool Overlap() const { return (flags_ >> 3) & 0x1; }
    void Overlap(const bool &overlap) { flags_ = (flags_ & 0xF7) | (overlap << 3); }

    uint8_t ScannerChannel() const { return (flags_ >> 4) & 0x03; }
    void ScannerChannel(const uint8_t &scanner_channel) {
        if (scanner_channel > 3)
            throw std::runtime_error("Scanner channel must be <= 3");
        else
            flags_ = (flags_ & 0xCF) | (scanner_channel << 4);
    }

    bool ScanDirectionFlag() const { return (flags_ >> 6) & 0x1; }
    void ScanDirectionFlag(const bool &scan_dir_flag) { flags_ = (flags_ & 0xBF) | (scan_dir_flag << 6); }

    bool EdgeOfFlightLineFlag() const { return flags_ >> 7; }
    void EdgeOfFlightLineFlag(const bool &eof_flag) { flags_ = (flags_ & 0x7F) | (eof_flag << 7); }

    uint8_t Classification() const { return classification_; }
    void Classification(const uint8_t &classification) { classification_ = classification; }

    uint8_t UserData() const { return user_data_; }
    void UserData(const uint8_t &user_data) { user_data_ = user_data; }

    int16_t ScanAngle() const { return scan_angle_; }
    void ScanAngle(const int16_t &scan_angle) {
        if (scan_angle < -30000 || scan_angle > 30000)
            throw std::runtime_error("Scan Angle must be between -30000 and 30000");
        else
            scan_angle_ = scan_angle;
    }

    uint16_t PointSourceId() const { return point_source_id_; }
    void PointSourceId(const uint16_t &point_source_ID) { point_source_id_ = point_source_ID; }

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
