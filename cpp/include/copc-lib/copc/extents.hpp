#ifndef COPCLIB_COPC_EXTENTS_H_
#define COPCLIB_COPC_EXTENTS_H_

#include <string>
#include <vector>

#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{

class CopcExtent : public las::CopcExtentsVlr::CopcExtent
{
  public:
    CopcExtent();

    CopcExtent(double minimum, double maximum);

    CopcExtent(const std::vector<double> &vec);

    CopcExtent(const las::CopcExtentsVlr::CopcExtent &other);

    // Convert to lazperf CopcExtent
    las::CopcExtentsVlr::CopcExtent ToLazPerf() const { return {minimum, maximum}; }

    std::string ToString() const;
};

class CopcExtents
{
  public:
    // Empty constructor
    CopcExtents(int8_t point_format_id, uint16_t num_eb_items = 0);

    // VLR constructor
    CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_eb_items = 0);

    // Getters
    int8_t PointFormatID() const { return point_format_id_; }
    // Get all extents as a vector
    std::vector<std::shared_ptr<CopcExtent>> Extents() { return extents_; }

    std::shared_ptr<CopcExtent> X() { return extents_[0]; }
    std::shared_ptr<CopcExtent> Y() { return extents_[1]; }
    std::shared_ptr<CopcExtent> Z() { return extents_[2]; }
    std::shared_ptr<CopcExtent> Intensity() { return extents_[3]; }
    std::shared_ptr<CopcExtent> ReturnNumber() { return extents_[4]; }
    std::shared_ptr<CopcExtent> NumberOfReturns() { return extents_[5]; }
    std::shared_ptr<CopcExtent> ScannerChannel() { return extents_[6]; }
    std::shared_ptr<CopcExtent> ScanDirectionFlag() { return extents_[7]; }
    std::shared_ptr<CopcExtent> EdgeOfFlightLine() { return extents_[8]; }
    std::shared_ptr<CopcExtent> Classification() { return extents_[9]; }
    std::shared_ptr<CopcExtent> UserData() { return extents_[10]; }
    std::shared_ptr<CopcExtent> ScanAngle() { return extents_[11]; }
    std::shared_ptr<CopcExtent> PointSourceID() { return extents_[12]; }
    std::shared_ptr<CopcExtent> GpsTime() { return extents_[13]; }

    std::shared_ptr<CopcExtent> Red()
    {
        if (point_format_id_ > 6)
            return extents_[14];
        else
            throw std::runtime_error("CopcExtents::Red: This point format does not have Red");
    }
    std::shared_ptr<CopcExtent> Green()
    {
        if (point_format_id_ > 6)
            return extents_[15];
        else
            throw std::runtime_error("CopcExtents::Green: This point format does not have Green");
    }
    std::shared_ptr<CopcExtent> Blue()
    {
        if (point_format_id_ > 6)
            return extents_[16];
        else
            throw std::runtime_error("CopcExtents::Blue: This point format does not have Blue");
    }

    std::shared_ptr<CopcExtent> NIR()
    {
        if (point_format_id_ == 8)
            return extents_[17];
        else
            throw std::runtime_error("CopcExtents::NIR: This point format does not have NIR");
    }

    std::vector<std::shared_ptr<CopcExtent>> ExtraBytes()
    {
        std::vector<std::shared_ptr<CopcExtent>> extra_bytes;
        extra_bytes.reserve(extents_.size() - las::PointBaseNumberDimensions(point_format_id_));
        extra_bytes.assign(extents_.begin() + las::PointBaseNumberDimensions(point_format_id_), extents_.end());
        return extra_bytes;
    }

    // Setters for python bindings
    void X(std::shared_ptr<CopcExtent> x) { extents_[0] = x; }
    void Y(std::shared_ptr<CopcExtent> y) { extents_[1] = y; }
    void Z(std::shared_ptr<CopcExtent> z) { extents_[2] = z; }
    void Intensity(std::shared_ptr<CopcExtent> intensity) { extents_[3] = intensity; }
    void ReturnNumber(std::shared_ptr<CopcExtent> return_number) { extents_[4] = return_number; }
    void NumberOfReturns(std::shared_ptr<CopcExtent> number_of_returns) { extents_[5] = number_of_returns; }
    void ScannerChannel(std::shared_ptr<CopcExtent> scanner_channel) { extents_[6] = scanner_channel; }
    void ScanDirectionFlag(std::shared_ptr<CopcExtent> scan_direction_flag) { extents_[7] = scan_direction_flag; }
    void EdgeOfFlightLine(std::shared_ptr<CopcExtent> edge_of_flight_line) { extents_[8] = edge_of_flight_line; }
    void Classification(std::shared_ptr<CopcExtent> classification) { extents_[9] = classification; }
    void UserData(std::shared_ptr<CopcExtent> user_data) { extents_[10] = user_data; }
    void ScanAngle(std::shared_ptr<CopcExtent> scan_angle) { extents_[11] = scan_angle; }
    void PointSourceID(std::shared_ptr<CopcExtent> point_source_id) { extents_[12] = point_source_id; }
    void GpsTime(std::shared_ptr<CopcExtent> gps_time) { extents_[13] = gps_time; }
    void Red(std::shared_ptr<CopcExtent> red)
    {
        if (point_format_id_ > 6)
            extents_[14] = red;
        else
            throw std::runtime_error("CopcExtents::Red: This point format does not have Red");
    }
    void Green(std::shared_ptr<CopcExtent> green)
    {
        if (point_format_id_ > 6)
            extents_[15] = green;
        else
            throw std::runtime_error("CopcExtents::Green: This point format does not have Green");
    }
    void Blue(std::shared_ptr<CopcExtent> blue)
    {
        if (point_format_id_ > 6)
            extents_[16] = blue;
        else
            throw std::runtime_error("CopcExtents::Blue: This point format does not have Blue");
    }

    void NIR(std::shared_ptr<CopcExtent> nir)
    {
        if (point_format_id_ == 8)
            extents_[17] = nir;
        else
            throw std::runtime_error("CopcExtents::NIR: This point format does not have NIR");
    }

    void ExtraBytes(std::vector<std::shared_ptr<CopcExtent>> extra_bytes)
    {
        if (extra_bytes.size() != (extents_.size() - las::PointBaseNumberDimensions(point_format_id_)))
            throw std::runtime_error("CopcExtents::ExtraBytes: Vector of extra byte must be the right length.");
        std::copy(extra_bytes.begin(), extra_bytes.end(),
                  extents_.begin() + las::PointBaseNumberDimensions(point_format_id_));
    }

    // Set all extents as a vector
    void Extents(std::vector<std::shared_ptr<CopcExtent>> extents)
    {
        if (extents.size() != extents_.size())
            throw std::runtime_error("CopcExtents::Extents: Vector of extents must be the right length.");
        extents_ = extents;
    }

    // Convert to lazperf vlr
    las::CopcExtentsVlr ToCopcExtentsVlr() const;

    // Set all extents from vector

    // Return the total number of extents
    static int NumberOfExtents(int8_t point_format_id, uint16_t num_eb_items);

    // Return the size in bytes of the extents
    static size_t GetByteSize(int8_t point_format_id, uint16_t num_eb_items);

    std::string ToString() const;

  private:
    int8_t point_format_id_;
    std::vector<std::shared_ptr<CopcExtent>> extents_;
};
} // namespace copc
#endif // COPCLIB_COPC_EXTENTS_H_
