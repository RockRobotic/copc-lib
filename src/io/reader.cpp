#include <copc-lib/io/reader.hpp>
#include <copc-lib/las/header.hpp>

#include <lazperf/readers.hpp>

namespace copc::io
{
Reader::Reader(std::istream &in_stream) : in_stream(in_stream)
{
    if (!this->in_stream.good())
        throw std::runtime_error("Invalid input stream!");

    this->reader_ = std::make_unique<lazperf::reader::generic_file>(this->in_stream);
    auto header = this->reader_->header();

    ReadVlrs();

    copc_data = GetCopcData();
    auto wkt = GetWktData();

    this->file = std::make_shared<CopcFile>(header, copc_data.span, wkt.wkt);
}

void Reader::ReadVlrs()
{
    // Move stream to beginning of VLRs
    this->in_stream.seekg(this->reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs_` list
    size_t count = 0;
    while (count < this->reader_->header().vlr_count && this->in_stream.good() && !this->in_stream.eof())
    {
        uint64_t cur_pos = this->in_stream.tellg();
        las::VlrHeader h = las::VlrHeader::create(this->in_stream);
        vlrs_[cur_pos] = h;

        this->in_stream.seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
}

las::CopcVlr io::Reader::GetCopcData()
{
    this->in_stream.seekg(COPC_OFFSET);
    las::CopcVlr copc = las::CopcVlr::create(this->in_stream);
    return copc;
}

las::WktVlr io::Reader::GetWktData()
{
    this->in_stream.seekg(copc_data.wkt_vlr_offset);
    las::WktVlr wkt = las::WktVlr::create(this->in_stream, copc_data.wkt_vlr_size);
    return wkt;
}
} // namespace copc::io