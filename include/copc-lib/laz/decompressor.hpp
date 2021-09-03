#ifndef COPCLIB_LAZ_DECOMPRESS_H_
#define COPCLIB_LAZ_DECOMPRESS_H_

#include <copc-lib/las/header.hpp>

#include <lazperf/filestream.hpp>
#include <lazperf/readers.hpp>

#include <istream>
#include <vector>

using namespace lazperf;

namespace copc::laz
{

class Decompressor
{
  public:
    // Decompresses bytes from the instream and returns them
    static std::vector<char> DecompressBytes(std::istream &in_stream, las::LasHeader &header, int point_count)
    {
        std::vector<char> out;

        InFileStream stre(in_stream);
        las_decompressor::ptr decompressor =
            build_las_decompressor(stre.cb(), header.point_format_id, header.ebCount());

        int point_size = header.point_record_length;
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
};
} // namespace copc::laz

#endif // COPCLIB_LAZ_DECOMPRESS_H_
