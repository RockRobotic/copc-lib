#include "copc-lib/io/base_reader.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/copc/extents.hpp"
#include "copc-lib/las/header.hpp"

#include <lazperf/readers.hpp>
#include <lazperf/vlr.hpp>

namespace copc
{
void BaseReader::InitReader()
{
    if (!in_stream_->good())
        throw std::runtime_error("Invalid input stream!");

    reader_ = std::make_unique<lazperf::reader::generic_file>(*in_stream_);

    auto header = las::LasHeader::FromLazPerf(reader_->header());

    // Load vlrs and evlrs
    vlrs_ = ReadVlrHeaders();
    auto wkt = ReadWktVlr(vlrs_);
    auto eb = ReadExtraBytesVlr(vlrs_);

    las_config_ = las::LazConfig(header, wkt.wkt, eb);
}

std::map<uint64_t, las::VlrHeader> BaseReader::ReadVlrHeaders()
{
    std::map<uint64_t, las::VlrHeader> out;

    // Move stream to beginning of VLRs
    in_stream_->seekg(reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs` list
    for (int i = 0; i < reader_->header().vlr_count; i++)
    {
        uint64_t cur_pos = in_stream_->tellg();
        auto h = las::VlrHeader(lazperf::vlr_header::create(*in_stream_));
        out.insert({cur_pos, h});

        in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
    }

    // Move stream to beginning of EVLRs
    in_stream_->seekg(reader_->header().evlr_offset);

    // Iterate through all vlr's and add them to the `vlrs` list
    for (int i = 0; i < reader_->header().evlr_count; i++)
    {
        uint64_t cur_pos = in_stream_->tellg();
        auto h = las::VlrHeader(lazperf::evlr_header::create(*in_stream_));
        out.insert({cur_pos, h});

        in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
    }

    return out;
}
las::WktVlr BaseReader::ReadWktVlr(std::map<uint64_t, las::VlrHeader> &vlrs)
{
    auto offset = FetchVlr(vlrs, "LASF_Projection", 2112);
    if (offset != 0)
    {
        in_stream_->seekg(offset + lazperf::evlr_header::Size);
        return las::WktVlr::create(*in_stream_, static_cast<int>(vlrs[offset].data_length));
    }
    return las::WktVlr();
}

las::EbVlr BaseReader::ReadExtraBytesVlr(std::map<uint64_t, las::VlrHeader> &vlrs)
{
    auto offset = FetchVlr(vlrs, "LASF_Spec", 4);
    if (offset != 0)
    {
        in_stream_->seekg(offset + lazperf::vlr_header::Size);
        return las::EbVlr::create(*in_stream_, static_cast<int>(vlrs[offset].data_length));
    }
    return las::EbVlr();
}

uint64_t BaseReader::FetchVlr(const std::map<uint64_t, las::VlrHeader> &vlrs, const std::string &user_id,
                              uint16_t record_id)
{
    for (auto &[offset, vlr_header] : vlrs)
    {
        if (vlr_header.user_id == user_id && vlr_header.record_id == record_id)
            return offset;
    }
    return 0;
}

} // namespace copc
