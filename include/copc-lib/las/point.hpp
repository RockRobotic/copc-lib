#ifndef COPCLIB_LAS_POINT_H_
#define COPCLIB_LAS_POINT_H_

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <copc-lib/las/internal/extra_bytes.hpp>
#include <copc-lib/las/internal/gps_time.hpp>
#include <copc-lib/las/internal/nir.hpp>
#include <copc-lib/las/internal/point_10.hpp>
#include <copc-lib/las/internal/point_14.hpp>
#include <copc-lib/las/internal/rgb.hpp>
#include <copc-lib/las/internal/utils.hpp>

namespace copc::las {

class Point {
public:
    Point(const uint8_t &point_format_id);

    // Copy constructor
    Point(const Point &point);

    void Unpack(std::istream &in_stream, const uint16_t &point_record_length);

    void Pack(std::ostream &out_stream) const;

    static uint8_t BaseByteSize(const uint8_t &point_format_id);

    std::string ToString() const;

    void ToPointFormat(const uint8_t &point_format_id);

    // Getters and Setters
    int32_t X() const { return point_10_ ? point_10_->X() : point_14_->X(); }
    void X(const int32_t &x) {
        if (point_10_)
            point_10_->X(x);
        else
            point_14_->X(x);
    }

    int32_t Y() const { return point_10_ ? point_10_->Y() : point_14_->Y(); }
    void Y(const int32_t &y) {
        if (point_10_)
            point_10_->Y(y);
        else
            point_14_->Y(y);
    }

    int32_t Z() const { return point_10_ ? point_10_->Z() : point_14_->Z(); }
    void Z(const int32_t &z) {
        if (point_10_)
            point_10_->Z(z);
        else
            point_14_->Z(z);
    }

    uint16_t Intensity() const { return point_10_ ? point_10_->Intensity() : point_14_->Intensity(); }
    void Intensity(const uint16_t &intensity) {
        if (point_10_)
            point_10_->Intensity(intensity);
        else
            point_14_->Intensity(intensity);
    }

    uint8_t ReturnsScanDirEofBitFields() const {
        if (point_14_)
            throw std::runtime_error("Point14 does not have ReturnsScanDirEofBitFields, use ReturnsBitFields instead.");
        else
            return point_10_->ReturnsScanDirEofBitFields();
    }
    void ReturnsScanDirEofBitFields(const uint8_t &returns_bit_fields) {
        if (point_14_)
            throw std::runtime_error("Point14 does not have ReturnsScanDirEofBitFields, use ReturnsBitFields instead.");
        else
            point_10_->ReturnsScanDirEofBitFields(returns_bit_fields);
    }

    uint8_t ReturnsBitFields() const {
        if (point_10_)
            throw std::runtime_error("Point10 does not have ReturnsBitFields, use ReturnsScanDirEofBitFields instead.");
        else
            return point_14_->ReturnsBitFields();
    }
    void ReturnsBitFields(const uint8_t &returns_bit_fields) {
        if (point_10_)
            throw std::runtime_error("Point10 does not have ReturnsBitFields, use ReturnsScanDirEofBitFields instead.");
        else
            point_14_->ReturnsBitFields(returns_bit_fields);
    }

    uint8_t ReturnNumber() const { return point_10_ ? point_10_->ReturnNumber() : point_14_->ReturnNumber(); }
    void ReturnNumber(const uint8_t &return_number) {
        if (point_10_)
            point_10_->ReturnNumber(return_number);
        else
            point_14_->ReturnNumber(return_number);
    }

    uint8_t NumberOfReturns() const { return point_10_ ? point_10_->NumberOfReturns() : point_14_->NumberOfReturns(); }
    void NumberOfReturns(const uint8_t &number_of_returns) {
        if (point_10_)
            point_10_->NumberOfReturns(number_of_returns);
        else
            point_14_->NumberOfReturns(number_of_returns);
    }

    uint8_t FlagsBitFields() const {
        if (point_10_)
            throw std::runtime_error("Point10 does not have FlagsBitFields.");
        else
            return point_14_->FlagsBitFields();
    }
    void FlagsBitFields(const uint8_t &class_flags) {
        if (point_10_)
            throw std::runtime_error("Point10 does not have FlagsBitFields.");
        else
            point_14_->FlagsBitFields(class_flags);
    }

    bool Synthetic() const { return point_10_ ? point_10_->Synthetic() : point_14_->Synthetic(); }
    void Synthetic(const bool &synthetic) {
        point_10_ ? point_10_->Synthetic(synthetic) : point_14_->Synthetic(synthetic);
    }

    bool KeyPoint() const { return point_10_ ? point_10_->KeyPoint() : point_14_->KeyPoint(); }
    void KeyPoint(const bool &key_point) {
        point_10_ ? point_10_->KeyPoint(key_point) : point_14_->KeyPoint(key_point);
    }

    bool Withheld() const { return point_10_ ? point_10_->Withheld() : point_14_->Withheld(); }
    void Withheld(const bool &withheld) { point_10_ ? point_10_->Withheld(withheld) : point_14_->Withheld(withheld); }

    bool Overlap() const {
        if (point_10_)
            throw std::runtime_error("Point10 does not have Overlap.");
        else
            return point_14_->Overlap();
    }
    void Overlap(const bool &overlap) {
        if (point_10_)
            throw std::runtime_error("Point10 does not have Overlap.");
        else
            point_14_->Overlap(overlap);
    }

    uint8_t ScannerChannel() const {
        if (point_10_)
            throw std::runtime_error("Point10 does not have Scanner Channel.");
        else
            return point_14_->ScannerChannel();
    }
    void ScannerChannel(const uint8_t &scanner_channel) {
        if (point_10_)
            throw std::runtime_error("Point10 does not have Scanner Channel.");
        else
            point_14_->ScannerChannel(scanner_channel);
    }

    bool ScanDirFlag() const { return point_10_ ? point_10_->ScanDirectionFlag() : point_14_->ScanDirectionFlag(); }
    void ScanDirFlag(const bool &scan_dir_flag) {
        if (point_10_)
            point_10_->ScanDirectionFlag(scan_dir_flag);
        else
            point_14_->ScanDirectionFlag(scan_dir_flag);
    }

    bool EdgeOfFlightLineFlag() const {
        return point_10_ ? point_10_->EdgeOfFlightLineFlag() : point_14_->EdgeOfFlightLineFlag();
    }
    void EdgeOfFlightLineFlag(const bool &eofl_flag) {
        if (point_10_)
            point_10_->EdgeOfFlightLineFlag(eofl_flag);
        else
            point_14_->EdgeOfFlightLineFlag(eofl_flag);
    }

    uint8_t ClassificationBitFields() const {
        if (point_10_)
            return point_10_->ClassificationBitFields();
        else
            throw std::runtime_error("Point14 does not have Classification BitFields.");
    }

    void ClassificationBitFields(const uint8_t &classification_bitfields) {
        if (point_10_)
            point_10_->ClassificationBitFields(classification_bitfields);
        else
            throw std::runtime_error("Point14 does not have Classification Bit Fields.");
    }

    uint8_t Classification() const { return point_10_ ? point_10_->Classification() : point_14_->Classification(); }
    void Classification(const uint8_t &classification) {
        if (point_10_)
            point_10_->Classification(classification);
        else
            point_14_->Classification(classification);
    }

    int8_t ScanAngleRank() const {
        if (point_10_)
            return point_10_->ScanAngleRank();
        else
            throw std::runtime_error("Point14 does not have Scan Angle Rank.");
    }

    void ScanAngleRank(const int8_t &scan_angle) {
        if (point_10_)
            point_10_->ScanAngleRank(scan_angle);
        else
            throw std::runtime_error("Point14 does not have Scan Angle Rank.");
    }

    int16_t ScanAngle() const {
        if (point_10_)
            throw std::runtime_error("Point10 does not have Scan Angle.");
        else
            return point_14_->ScanAngle();
    }

    void ScanAngle(const int16_t &scan_angle) {
        if (point_10_)
            throw std::runtime_error("Point10 does not have Scan Angle.");
        else
            point_14_->ScanAngle(scan_angle);
    }

    uint8_t UserData() const { return point_10_ ? point_10_->UserData() : point_14_->UserData(); }
    void UserData(const uint8_t &user_data) {
        if (point_10_)
            point_10_->UserData(user_data);
        else
            point_14_->UserData(user_data);
    }

    uint16_t PointSourceId() const { return point_10_ ? point_10_->PointSourceId() : point_14_->PointSourceId(); }
    void PointSourceId(const uint16_t &point_source_id) {
        if (point_10_)
            point_10_->PointSourceId(point_source_id);
        else
            point_14_->PointSourceId(point_source_id);
    }

    void SetRgb(const uint16_t &r, const uint16_t &g, const uint16_t &b) {
        if (rgb_)
            rgb_->SetRgb(r, g, b);
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t R() const {
        if (rgb_)
            return rgb_->R();
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void R(const uint16_t &r) {
        if (rgb_)
            rgb_->R(r);
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t G() const {
        if (rgb_)
            return rgb_->G();
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void G(const uint16_t g) {
        if (rgb_)
            rgb_->G(g);
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    uint16_t B() const {
        if (rgb_)
            return rgb_->B();
        else
            throw std::runtime_error("This point format does not have RGB");
    }
    void B(const uint16_t &b) {
        if (rgb_)
            rgb_->B(b);
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    double GetGpsTimeVal() const {
        if (gps_time_)
            return gps_time_->Value();
        else
            throw std::runtime_error("This point format does not have GPS time.");
    }
    void SetGpsTimeVal(const double &gps_time) {
        if (gps_time_)
            gps_time_->Value(gps_time);
        else
            throw std::runtime_error("This point format does not have GPS time.");
    }

    uint16_t GetNirVal() const {
        if (nir_)
            return nir_->Value();
        else
            throw std::runtime_error("This point format does not have NIR.");
    }
    void SetNirVal(const uint16_t &nir_val) {
        if (nir_)
            nir_->Value(nir_val);
        else
            throw std::runtime_error("This point format does not have NIR.");
    }

    bool HasPoint10() const { return point_10_ != nullptr; }

    internal::Point10 GetPoint10() const {
        if (point_10_)
            return *point_10_;
        else
            throw std::runtime_error("This point format does not have Point10.");
    }

    bool HasPoint14() const { return point_14_ != nullptr; }

    internal::Point14 GetPoint14() const {
        if (point_14_)
            return *point_14_;
        else
            throw std::runtime_error("This point format does not have Point14.");
    }

    bool HasGpsTime() const { return gps_time_ != nullptr; }

    internal::GpsTime GetGpsTime() const {
        if (gps_time_)
            return *gps_time_;
        else
            throw std::runtime_error("This point format does not have GPS Time.");
    }

    bool HasRgb() const { return rgb_ != nullptr; }

    internal::Rgb GetRgb() const {
        if (rgb_)
            return *rgb_;
        else
            throw std::runtime_error("This point format does not have RGB.");
    }

    bool HasNir() const { return nir_ != nullptr; }

    internal::Nir GetNir() const {
        if (nir_)
            return *nir_;
        else
            throw std::runtime_error("This point format does not have NIR.");
    }

    ~Point() {
        delete point_10_;
        delete point_14_;
        delete gps_time_;
        delete rgb_;
        delete nir_;
    }

protected:
    internal::Point10 *point_10_;
    internal::Point14 *point_14_;
    internal::GpsTime *gps_time_;
    internal::Rgb *rgb_;
    internal::Nir *nir_;
    std::vector<internal::ExtraByte> extra_bytes_;
    uint16_t point_byte_size_;
    uint8_t point_format_id_;
};

} // namespace copc::las
#endif // COPCLIB_LAS_POINT_H_