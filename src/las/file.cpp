#include <copc-lib/las/file.hpp>

namespace copc::las
{

LasFile::LasFile(std::istream *in_stream) : in_stream_(in_stream)
{
    if (!this->in_stream_->good())
        throw std::runtime_error("Invalid input stream!");

    this->reader_ = std::make_unique<lazperf::reader::generic_file>(*this->in_stream_);
    this->header_ = this->reader_->header();

    ReadVlrs();
}

LasFile::LasFile(LasHeader header)
{
    this->header_ = header;
}

void LasFile::ReadVlrs()
{
    // Move stream to beginning of VLRs
    this->in_stream_->seekg(this->reader_->header().header_size);

    // Iterate through all vlr's and add them to the `vlrs_` list
    size_t count = 0;
    while (count < this->reader_->header().vlr_count && this->in_stream_->good() && !this->in_stream_->eof())
    {
        uint64_t cur_pos = this->in_stream_->tellg();
        VlrHeader h = VlrHeader::create(*this->in_stream_);
        vlrs_[cur_pos] = h;

        this->in_stream_->seekg(h.data_length, std::ios::cur); // jump foward
        count++;
    }
}
} // namespace copc::las