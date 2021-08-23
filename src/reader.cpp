#include "reader.hpp"

#include <cstring>
#include <stdexcept>

namespace copc
{
namespace io
{

CopcReader::CopcReader(istream &in_stream) : stream_(in_stream)
{
    this->reader_ = make_unique<lazperf::reader::generic_file>(this->stream_);

    ReadVlrs();
    ReadCopcVlr();
}

std::vector<Entry> CopcReader::ReadPage(const int64_t &offset, const uint64_t &pageSize)
{
    if (pageSize % sizeof(Entry) != 0)
        throw runtime_error("invalid page size!");

    // ignore for now since we can't load evlrs
    // if (this->vlrs_.find(offset) == this->vlrs_.end())
    //    throw runtime_error("invalid offset!");

    std::vector<Entry> out;

    int num_pages = int(pageSize / sizeof(Entry));

    this->stream_.seekg(offset);
    for (int i = 0; i < num_pages; i++)
    {
        out.push_back(ReadEntry());
    }
    return out;
}

Entry CopcReader::ReadEntry()
{
    Entry out;

    this->stream_.read(reinterpret_cast<char *>(&out.key.d), sizeof(out.key.d));
    this->stream_.read(reinterpret_cast<char *>(&out.key.x), sizeof(out.key.x));
    this->stream_.read(reinterpret_cast<char *>(&out.key.y), sizeof(out.key.y));
    this->stream_.read(reinterpret_cast<char *>(&out.key.z), sizeof(out.key.z));

    this->stream_.read(reinterpret_cast<char *>(&out.offset), sizeof(out.offset));
    this->stream_.read(reinterpret_cast<char *>(&out.byteSize), sizeof(out.byteSize));
    this->stream_.read(reinterpret_cast<char *>(&out.pointCount), sizeof(out.pointCount));

    return out;
}

void CopcReader::ReadVlrs()
{
    this->stream_.seekg(this->reader_->header().header_size);

    size_t count = 0;
    while (count < this->reader_->header().vlr_count && this->stream_.good() && !this->stream_.eof())
    {
        uint64_t cur_pos = this->stream_.tellg();
        VlrHeader h = VlrHeader::create(this->stream_);
        vlrs_[cur_pos] = h;

        this->stream_.seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
}

void CopcReader::ReadCopcVlr()
{
    int64_t copc_offset = this->reader_->header().header_size;

    if (this->vlrs_.find(copc_offset) == this->vlrs_.end())
        throw runtime_error("COPC vlr not found!");

    this->stream_.seekg(copc_offset + VlrHeader::Size);

    CopcVlr copc = CopcVlr::create(this->stream_);
    copc_data_ = CopcData(copc);
}

void CopcReader::ReadWkt()
{
    wkt_loaded_ = true;
    for (auto const &[offset, vlr] : vlrs_)
    {
        if (vlr.user_id == "LASF_Projection" && vlr.record_id == 2112)
        {
            this->stream_.seekg(offset + VlrHeader::Size);
            WktVlr wktVlr = WktVlr::create(this->stream_, vlr.data_length);
            wkt_data_ = wktVlr.wkt;
            return;
        }
    }
}

std::vector<char> CopcReader::GetPoints(Entry entry)
{
    std::vector<char> out;
    if (!entry.IsValid())
        return out;

    this->stream_.seekg(entry.offset);

    InFileStream stre(this->stream_);
    las_decompressor::ptr decompressor =
        build_las_decompressor(stre.cb(), this->reader_->header().point_format_id, this->reader_->header().ebCount());

    int point_size = this->reader_->header().point_record_length;
    char buff[255];
    for (int i = 0; i < entry.pointCount; i++)
    {
        decompressor->decompress(buff);
        out.insert(out.end(), buff, buff + point_size);
    }
    return out;
}

} // namespace io
} // namespace copc