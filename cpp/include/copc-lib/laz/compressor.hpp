#ifndef COPCLIB_LAZ_COMPRESS_H_
#define COPCLIB_LAZ_COMPRESS_H_

#include <istream>
#include <stdexcept>
#include <vector>

#include <lazperf/filestream.hpp>

#include "copc-lib/io/copc_writer.hpp"
#include "copc-lib/las/utils.hpp"

using namespace lazperf;

namespace copc::laz
{

class Compressor
{
  public:
    // Compresses bytes and writes them to the out stream
    static int32_t CompressBytes(std::ostream &out_stream, const int8_t &point_format_id, const uint16_t &eb_byte_size,
                                 const std::vector<char> &in)
    {
        OutFileStream stream(out_stream);

        las_compressor::ptr compressor = build_las_compressor(stream.cb(), point_format_id, eb_byte_size);

        int point_size = copc::las::PointByteSize(point_format_id, eb_byte_size);
        if (in.size() % point_size != 0)
            throw std::runtime_error("Invalid input stream for compression!");
        if (in.size() > std::numeric_limits<int32_t>::max())
            throw std::runtime_error("Input byte stream is too large - split into multiple chunks!");

        int32_t point_count = static_cast<int32_t>(in.size()) / point_size;

        for (int i = 0; i < point_count; i++)
        {
            std::vector<char> buff =
                std::vector<char>(in.begin() + (i * point_size), in.begin() + ((i + 1) * point_size));
            compressor->compress(buff.data());
        }
        compressor->done();
        return point_count;
    }

    static int32_t CompressBytes(std::ostream &out_stream, las::LasHeader const &header, const std::vector<char> &in)
    {
        return CompressBytes(out_stream, header.PointFormatId(), header.EbByteSize(), in);
    }

    static std::vector<char> CompressBytes(const std::vector<char> &in, const int8_t &point_format_id,
                                           const uint16_t &eb_byte_size)
    {
        std::ostringstream out_stream;
        CompressBytes(out_stream, point_format_id, eb_byte_size, in);
        auto ostr = out_stream.str();
        return std::vector<char>(ostr.begin(), ostr.end());
    }

    static std::vector<char> CompressBytes(std::vector<char> &in, const las::LasHeader &header)
    {
        return CompressBytes(in, header.PointFormatId(), header.EbByteSize());
    }
};
} // namespace copc::laz

#endif // COPCLIB_LAZ_COMPRESS_H_
