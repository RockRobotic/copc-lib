#include <copc-lib/copc/file.hpp>

namespace copc
{

CopcFile::CopcFile(std::istream &in_stream) : LasFile(in_stream)
{
    this->copc = GetCopcData();

    if (this->copc.wkt_vlr_offset > 0)
        this->wkt = GetWktData();
}

las::CopcVlr CopcFile::GetCopcData()
{
    this->in_stream_.seekg(COPC_OFFSET);
    las::CopcVlr copc = las::CopcVlr::create(this->in_stream_);
    return copc;
}

las::WktVlr CopcFile::GetWktData()
{
    this->in_stream_.seekg(copc.wkt_vlr_offset);
    las::WktVlr wkt = las::WktVlr::create(this->in_stream_, copc.wkt_vlr_size);
    return wkt;
}

// CopcFile::CopcFile(std::string wkt) { this->wkt_ = wkt; }

} // namespace copc
