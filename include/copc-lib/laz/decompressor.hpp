#ifndef COPCLIB_LAZ_DECOMPRESS_H_
#define COPCLIB_LAZ_DECOMPRESS_H_

#include <copc-lib/io/reader.hpp>

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
    static std::vector<char> DecompressBytes(std::shared_ptr<io::Reader> reader, int point_count)
    {
        std::vector<char> out;

        InFileStream stre(reader->in_stream);
        las_decompressor::ptr decompressor = build_las_decompressor(
            stre.cb(), reader->file->GetLasHeader().point_format_id, reader->file->GetLasHeader().ebCount());

        int point_size = reader->file->GetLasHeader().point_record_length;
        char buff[255];
        for (int i = 0; i < point_count; i++)
        {
            decompressor->decompress(buff);
            out.insert(out.end(), buff, buff + point_size);
        }
        return out;
    }
};
} // namespace copc::laz

#endif // COPCLIB_LAZ_DECOMPRESS_H_