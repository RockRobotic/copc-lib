#ifndef COPCLIB_LAS_INTERNAL_POINT_10_H_
#define COPCLIB_LAS_INTERNAL_POINT_10_H_

#include <sstream>

#include <copc-lib/las/internal/utils.hpp>


namespace copc::las::internal {

class Point14;
class Point10 {

public:
    Point10()
        : x_(0), y_(0), z_(0), intensity_(0), returns_flags_eof_(0), classification_(0), scan_angle_rank_(0),
          user_data_(0), point_source_id_(0) {};

    Point10(int32_t x,
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
            uint16_t point_source_id);

    Point10(std::istream &in_stream) { Unpack(in_stream); }

    void Unpack(std::istream &in_stream);

    void Pack(std::ostream &out_stream) const;

    std::string ToString() const;

    Point14 ToPoint14() const;

    // Getters and Setters
    int32_t X() const { return x_; }
    void X(const int32_t &x) { x_ = x; }

    int32_t Y() const { return y_; }
    void Y(const int32_t &y) { y_ = y; }

    int32_t Z() const { return z_; }
    void Z(const int32_t &z) { z_ = z; }

    uint16_t Intensity() const { return intensity_; }
    void Intensity(const uint16_t &intensity) { intensity_ = intensity; }

    uint8_t ReturnsScanDirEofBitFields() const { return returns_flags_eof_; }
    void ReturnsScanDirEofBitFields(const uint8_t &returns_flags_eof) { returns_flags_eof_ = returns_flags_eof; }

    uint8_t ReturnNumber() const { return returns_flags_eof_ & 0x7; }
    void ReturnNumber(const uint8_t &return_number) {
        if (return_number > 7)
            throw std::runtime_error("Return Number must be <= 7");
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0xF8) | return_number;
    }

    uint8_t NumberOfReturns() const { return (returns_flags_eof_ >> 3) & 0x7; }
    void NumberOfReturns(const uint8_t &number_of_returns) {
        if (number_of_returns > 7)
            throw std::runtime_error("Number of Return must be <= 7");
        else
            returns_flags_eof_ = (returns_flags_eof_ & 0xC7) | (number_of_returns << 3);
    }

    bool ScanDirectionFlag() const { return (returns_flags_eof_ >> 6) & 0x1; }
    void ScanDirectionFlag(const bool &scan_direction_flag) {
        returns_flags_eof_ = (returns_flags_eof_ & 0xBF) | (scan_direction_flag << 6);
    }

    bool EdgeOfFlightLineFlag() const { return returns_flags_eof_ >> 7; }
    void EdgeOfFlightLineFlag(const bool &edge_of_flight_line) {
        returns_flags_eof_ = (returns_flags_eof_ & 0x7F) | (edge_of_flight_line << 7);
    }

    uint8_t ClassificationBitFields() const { return classification_; }
    void ClassificationBitFields(const uint8_t &classification) { classification_ = classification; }

    uint8_t Classification() const { return classification_ & 0x1F; }
    void Classification(const uint8_t &classification) {
        if (classification > 31)
            throw std::runtime_error("Classification must be <= 31");
        else
            classification_ = (classification_ & 0xE0) | classification;
    }

    bool Synthetic() const { return (classification_ >> 5) & 0x1; }
    void Synthetic(const bool &synthetic) { classification_ = (classification_ & 0xDF) | (synthetic << 5); }

    bool KeyPoint() const { return (classification_ >> 6) & 0x1; }
    void KeyPoint(const bool &key_point) { classification_ = (classification_ & 0xBF) | (key_point << 6); }

    bool Withheld() const { return (classification_ >> 7) & 0x1; }
    void Withheld(const bool &withheld) { classification_ = (classification_ & 0x7F) | (withheld << 7); }

    uint8_t UserData() const { return user_data_; }
    void UserData(const uint8_t &user_data) { user_data_ = user_data; }

    int8_t ScanAngleRank() const { return scan_angle_rank_; }
    void ScanAngleRank(const int8_t &scan_angle_rank) {
        if (scan_angle_rank < -90 || scan_angle_rank > 90)
            throw std::runtime_error("Scan Angle Rank must be between -90 and 90");
        else
            scan_angle_rank_ = scan_angle_rank;
    }

    uint16_t PointSourceId() const { return point_source_id_; }
    void PointSourceId(const uint16_t &point_source_ID) { point_source_id_ = point_source_ID; }

private:
    int32_t x_;
    int32_t y_;
    int32_t z_;
    uint16_t intensity_;
    uint8_t returns_flags_eof_;
    uint8_t classification_;
    int8_t scan_angle_rank_;
    uint8_t user_data_;
    uint16_t point_source_id_;
};
} // namespace copc::las::internal

#endif // COPCLIB_LAS_INTERNAL_POINT_10_H_
