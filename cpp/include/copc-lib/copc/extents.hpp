#ifndef COPCLIB_COPC_EXTENTS_H_
#define COPCLIB_COPC_EXTENTS_H_

#include <vector>

#include <copc-lib/las/utils.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{

class CopcExtent : public las::CopcExtentsVlr::CopcExtent
{
  public:
    CopcExtent() : las::CopcExtentsVlr::CopcExtent(0, 0){};

    CopcExtent(double minimum, double maximum);

    CopcExtent(const std::vector<double> &vec);

    CopcExtent(const las::CopcExtentsVlr::CopcExtent &other);

    // Convert to lazperf CopcExtent
    las::CopcExtentsVlr::CopcExtent ToLazPerf() const { return {minimum, maximum}; }
};

class CopcExtents
{
  public:
    // Empty constructor
    CopcExtents(int8_t point_format_id, uint16_t eb_count = 0);

    // VLR constructor
    CopcExtents(const las::CopcExtentsVlr &vlr, int8_t point_format_id, uint16_t eb_count = 0);

    // Getters/Setters
    void PointFormatID(int8_t point_format_id)
    {
        if (point_format_id < 6 || point_format_id > 8)
            throw std::runtime_error("CopcExtents::PointFormatID: Supported point formats are 6 to 8.");
        point_format_id_ = point_format_id;
    }
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

    // Convert to lazperf vlr
    las::CopcExtentsVlr ToCopcExtentsVlr() const;

    // Set all extents from vector

    // Return the total number of extents
    static int NumberOfExtents(int8_t point_format_id, uint16_t eb_count);

    // Return the size in bytes of the extents
    static size_t GetByteSize(int8_t point_format_id, uint16_t eb_count);

  private:
    int8_t point_format_id_;
    std::vector<std::shared_ptr<CopcExtent>> extents_;
};
} // namespace copc
#endif // COPCLIB_COPC_EXTENTS_H_
