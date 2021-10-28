#ifndef COPCLIB_LAZ_DECOMPRESS_H_
#define COPCLIB_LAZ_DECOMPRESS_H_

#include "copc-lib/las/header.hpp"

#include <lazperf/filestream.hpp>
#include <lazperf/readers.hpp>

#include <istream>
#include <sstream>
#include <vector>

using namespace lazperf;

namespace copc::laz
{

class Decompressor
{
  public:
    // Decompresses bytes from the instream and returns them
    static std::vector<char> DecompressBytes(std::istream &in_stream, const int8_t &point_format_id,
                                             const uint16_t &eb_byte_size, const int &point_count)
    {
        std::vector<char> out;

        InFileStream stre(in_stream);
        las_decompressor::ptr decompressor = build_las_decompressor(stre.cb(), point_format_id, eb_byte_size);

        int point_size = copc::las::PointByteSize(point_format_id, eb_byte_size);
        char buff[255];
        for (int i = 0; i < point_count; i++)
        {
            decompressor->decompress(buff);
            out.insert(out.end(), buff, buff + point_size);
        }
        // clear the EOF flag, since lazperf may read too large of a buffer
        in_stream.clear();
        return out;
    }

    static std::vector<char> DecompressBytes(std::istream &in_stream, const las::LasHeader &header,
                                             const int &point_count)
    {
        return DecompressBytes(in_stream, header.PointFormatId(), header.EbByteSize(), point_count);
    }

    static std::vector<char> DecompressBytes(const std::vector<char> &compressed_data, const int8_t &point_format_id,
                                             const uint16_t &eb_byte_size, const int &point_count)
    {
        std::istringstream in_stream(std::string(compressed_data.begin(), compressed_data.end()));
        return DecompressBytes(in_stream, point_format_id, eb_byte_size, point_count);
    }

    static std::vector<char> DecompressBytes(const std::vector<char> &compressed_data, const las::LasHeader &header,
                                             const int &point_count)
    {
        return DecompressBytes(compressed_data, header.PointFormatId(), header.EbByteSize(), point_count);
    }
};
} // namespace copc::laz

#endif // COPCLIB_LAZ_DECOMPRESS_H_
