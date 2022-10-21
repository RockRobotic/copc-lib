#ifndef COPCLIB_COPC_EXTENTS_H_
#define COPCLIB_COPC_EXTENTS_H_

#include <string>
#include <utility>
#include <vector>

#include <lazperf/vlr.hpp>

#include "copc-lib/las/utils.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{

namespace las
{
class CopcExtentsVlr;
}

class CopcExtent
{
  public:
    // TODO[Leo]: Make mean and var only accessible if has_extended_stats
    double minimum{0};
    double maximum{0};
    double mean{0};
    double var{1};

    CopcExtent() = default;

    CopcExtent(double minimum, double maximum, double mean = 0, double var = 1);

    CopcExtent(const std::vector<double> &vec);

    CopcExtent(const CopcExtent &other);

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, CopcExtent const &value)
    {
        os << value.ToString();
        return os;
    }

    // Operators
    bool operator==(const CopcExtent &other) const
    {
        return minimum == other.minimum && maximum == other.maximum && mean == other.mean && var == other.var;
    }
    bool operator!=(const CopcExtent &other) const { return !(*this == other); }
};

class CopcExtents
{
  public:
    // Empty constructor
    CopcExtents(int8_t point_format_id, uint16_t num_eb_items = 0, bool has_extended_stats = false);

    // Copy constructor
    CopcExtents(const CopcExtents &other);

    // Copy constructor with updated protected attributes
    CopcExtents(const CopcExtents &other, int8_t point_format_id, uint16_t num_eb_items, bool has_extended_stats);

    // VLR constructor
    CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t num_eb_items = 0,
                bool has_extended_stats = false);

    // Getters
    int8_t PointFormatId() const { return point_format_id_; }
    bool HasExtendedStats() const { return has_extended_stats_; }

    // Get all extents as a vector of shared_ptrs
    std::vector<std::shared_ptr<CopcExtent>> Extents() { return extents_; }

    // Get all extents as a copy vector
    std::vector<CopcExtent> Extents() const
    {
        std::vector<CopcExtent> vec;
        vec.reserve(extents_.size());

        for (int i{0}; i < extents_.size(); i++)
            vec.push_back(*extents_[i]);
        return vec;
    }

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
    std::shared_ptr<CopcExtent> PointSourceId() { return extents_[12]; }
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

    std::shared_ptr<CopcExtent> Nir()
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
    void X(std::shared_ptr<CopcExtent> x) { extents_[0] = std::move(x); }
    void Y(std::shared_ptr<CopcExtent> y) { extents_[1] = std::move(y); }
    void Z(std::shared_ptr<CopcExtent> z) { extents_[2] = std::move(z); }
    void Intensity(std::shared_ptr<CopcExtent> intensity) { extents_[3] = std::move(intensity); }
    void ReturnNumber(std::shared_ptr<CopcExtent> return_number) { extents_[4] = std::move(return_number); }
    void NumberOfReturns(std::shared_ptr<CopcExtent> number_of_returns) { extents_[5] = std::move(number_of_returns); }
    void ScannerChannel(std::shared_ptr<CopcExtent> scanner_channel) { extents_[6] = std::move(scanner_channel); }
    void ScanDirectionFlag(std::shared_ptr<CopcExtent> scan_direction_flag)
    {
        extents_[7] = std::move(scan_direction_flag);
    }
    void EdgeOfFlightLine(std::shared_ptr<CopcExtent> edge_of_flight_line)
    {
        extents_[8] = std::move(edge_of_flight_line);
    }
    void Classification(std::shared_ptr<CopcExtent> classification) { extents_[9] = std::move(classification); }
    void UserData(std::shared_ptr<CopcExtent> user_data) { extents_[10] = std::move(user_data); }
    void ScanAngle(std::shared_ptr<CopcExtent> scan_angle) { extents_[11] = std::move(scan_angle); }
    void PointSourceId(std::shared_ptr<CopcExtent> point_source_id) { extents_[12] = std::move(point_source_id); }
    void GpsTime(std::shared_ptr<CopcExtent> gps_time) { extents_[13] = std::move(gps_time); }
    void Red(std::shared_ptr<CopcExtent> red)
    {
        if (point_format_id_ > 6)
            extents_[14] = std::move(red);
        else
            throw std::runtime_error("CopcExtents::Red: This point format does not have Red");
    }
    void Green(std::shared_ptr<CopcExtent> green)
    {
        if (point_format_id_ > 6)
            extents_[15] = std::move(green);
        else
            throw std::runtime_error("CopcExtents::Green: This point format does not have Green");
    }
    void Blue(std::shared_ptr<CopcExtent> blue)
    {
        if (point_format_id_ > 6)
            extents_[16] = std::move(blue);
        else
            throw std::runtime_error("CopcExtents::Blue: This point format does not have Blue");
    }

    void Nir(std::shared_ptr<CopcExtent> nir)
    {
        if (point_format_id_ == 8)
            extents_[17] = std::move(nir);
        else
            throw std::runtime_error("CopcExtents::NIR: This point format does not have NIR");
    }

    void ExtraBytes(std::vector<std::shared_ptr<CopcExtent>> extra_bytes)
    {
        if (extra_bytes.size() != (extents_.size() - NumberOfExtents(point_format_id_)))
            throw std::runtime_error("CopcExtents::ExtraBytesVlr: Vector of extra byte must be the right length.");
        std::copy(extra_bytes.begin(), extra_bytes.end(), extents_.begin() + NumberOfExtents(point_format_id_));
    }

    // Convert to lazperf vlr
    las::CopcExtentsVlr ToLazPerf(const CopcExtent &x, const CopcExtent &y, const CopcExtent &z) const;

    // Convert to lazperf vlr extended (mean,var)
    las::CopcExtentsVlr ToLazPerfExtended() const;

    void SetExtendedStats(const las::CopcExtentsVlr &vlr);

    // Return the total number of extents
    size_t NumberOfExtents() const { return extents_.size(); }

    // Return the total number of extents
    static int NumberOfExtents(int8_t point_format_id, uint16_t num_eb_items = 0);

    // Return the size in bytes of the extents
    static size_t ByteSize(int8_t point_format_id, uint16_t num_eb_items);

    std::string ToString() const;
    friend std::ostream &operator<<(std::ostream &os, CopcExtents const &value)
    {
        os << value.ToString();
        return os;
    }

  private:
    int8_t point_format_id_;
    bool has_extended_stats_{false};
    std::vector<std::shared_ptr<CopcExtent>> extents_;
};

uint8_t PointBaseNumberExtents(const int8_t &point_format_id);

} // namespace copc
#endif // COPCLIB_COPC_EXTENTS_H_
