#include <cstring>

#include <copc-lib/io/writer.hpp>
#include <copc-lib/las/point.hpp>

#include <lazperf/filestream.hpp>
#include <lazperf/lazperf.hpp>

using namespace lazperf;

namespace copc::io
{

Writer::Writer(std::ostream &out_stream, LasConfig config) : out_stream(out_stream)
{
    auto header = HeaderFromConfig(config);
    this->file = std::make_shared<CopcFile>(header);

    char out_arr[FIRST_CHUNK_OFFSET];
    memset(out_arr, 0, sizeof(out_arr));
    out_stream.write(out_arr, sizeof(out_arr));
}

void Writer::Close(){
    WriteHeader();
    WriteChunkTable();
}

void Writer::WriteHeader()
{
    auto head14 = file->GetLasHeader();
    laz_vlr lazVlr(head14.pointFormat(), head14.ebCount(), VARIABLE_CHUNK_SIZE);
    //eb_vlr ebVlr(head14.ebCount());

    // Set the version number to 2 in order to write something reasonable.
    if (head14.version.minor < 2 || head14.version.minor > 4)
        head14.version.minor = 2;

    // point_format_id and point_record_length  are set on open().
    head14.header_size = head14.sizeFromVersion();
    head14.point_offset = head14.header_size;
    head14.vlr_count = 2; // copc + laz

    head14.point_format_id |= (1 << 7);
    head14.point_offset = OFFSET_TO_POINT_DATA;

    //if (head14.ebCount())
    //{
    //    head14.point_offset += ebVlr.size() + ebVlr.header().Size;
    //    head14.vlr_count++;
    //}

    if (head14.point_count_14 > (std::numeric_limits<uint32_t>::max)())
        head14.point_count = 0;
    else
        head14.point_count = (uint32_t)head14.point_count_14;
    // Set the WKT bit.
    head14.global_encoding |= (1 << 4);

    out_stream.seekp(0);
    head14.write(out_stream);

    // Write the VLR.
    file->GetCopcHeader().header().write(out_stream);
    file->GetCopcHeader().write(out_stream);

    lazVlr.header().write(out_stream);
    lazVlr.write(out_stream);

    //if (head14.ebCount())
    //{
    //    ebVlr.header().write(*f);
    //    ebVlr.write(*f);
    //}
}

void Writer::WriteChunkTable()
{
    // move to the end of the file to start emitting our compresed table
    out_stream.seekp(0, std::ios::end);

    // take note of where we're writing the chunk table, we need this later
    int64_t chunk_table_offset = static_cast<int64_t>(out_stream.tellp());

    // Fixup the chunk table to be relative offsets rather than absolute ones.
    uint64_t prevOffset = FIRST_CHUNK_OFFSET;
    for (auto &c : chunks_)
    {
        uint64_t relOffset = c.offset - prevOffset;
        prevOffset = c.offset;
        c.offset = relOffset;
    }

    // write out the chunk table header (version and total chunks)
    uint32_t version = 0;
    out_stream.write((const char *)&version, sizeof(uint32_t));
    uint32_t numChunks = (uint32_t)chunks_.size();
    out_stream.write((const char *)&numChunks, sizeof(uint32_t));

    // Write the chunk table
    OutFileStream w(out_stream);

    compress_chunk_table(w.cb(), chunks_, true);
    // go back to where we're supposed to write chunk table offset
    out_stream.seekp(OFFSET_TO_POINT_DATA);
    out_stream.write(reinterpret_cast<char *>(&chunk_table_offset), sizeof(chunk_table_offset));
}


las::LasHeader Writer::HeaderFromConfig(LasConfig config) { 
    
    las::LasHeader h;
    h.file_source_id = config.file_source_id;
    h.global_encoding = config.global_encoding;
    h.creation.day = config.creation.day;
    h.creation.year = config.creation.year;
    h.point_format_id = config.point_format_id;
    h.point_record_length = las::Point::BaseByteSize(config.point_format_id); // +extra_bytes;

    std::strcpy(h.guid, config.guid);
    std::strcpy(h.system_identifier, config.system_identifier);
    std::strcpy(h.generating_software, config.generating_software);

    h.offset.x = config.offset.x;
    h.offset.y = config.offset.y;
    h.offset.z = config.offset.z;

    h.scale.x = config.scale.x;
    h.scale.y = config.scale.y;
    h.scale.z = config.scale.z;

    h.maxx = 0;
    h.minx = 0;
    h.maxy = 0;
    h.miny = 0;
    h.maxz = 0;
    h.minz = 0;
    
    return h;
}
} // namespace copc::io