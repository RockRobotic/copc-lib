#include <cstring>
#include <iterator>
#include <stdexcept>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/io/internal/writer_internal.hpp"
#include "copc-lib/laz/compressor.hpp"

#include <lazperf/filestream.hpp>
#include <lazperf/lazperf.hpp>
#include <lazperf/vlr.hpp>

namespace copc::Internal
{

void WriterInternal::ComputeOffsetToPointData()
{

    size_t extents_offset =
        CopcExtents::GetByteSize(file_->GetLasHeader().point_format_id, file_->GetExtraBytes().items.size()) +
        lazperf::vlr_header::Size;

    size_t wkt_offset = file_->GetWkt().size() + lazperf::vlr_header::Size;

    size_t eb_offset = file_->GetExtraBytes().size();
    if (eb_offset > 0)
        eb_offset += lazperf::vlr_header::Size;

    OFFSET_TO_POINT_DATA += extents_offset + wkt_offset + eb_offset;
}

WriterInternal::WriterInternal(std::ostream &out_stream, const std::shared_ptr<CopcFile> &file,
                               std::shared_ptr<Hierarchy> hierarchy)
    : out_stream_(out_stream), file_(file), hierarchy_(hierarchy)
{
    // Update OFFSET_TO_POINT_DATA based on WKT and Extents
    ComputeOffsetToPointData();

    // reserve enough space for the header & VLRs in the file
    std::fill_n(std::ostream_iterator<char>(out_stream_), FIRST_CHUNK_OFFSET(), 0);
    open_ = true;
}

void WriterInternal::Close()
{
    if (!open_)
        return;

    auto head14 = file_->GetLasHeader();
    WriteChunkTable();

    // Set hierarchy evlr
    out_stream_.seekp(0, std::ios::end);
    head14.evlr_offset = static_cast<int64_t>(out_stream_.tellp());
    head14.evlr_count += hierarchy_->seen_pages_.size();

    // Page writing must be done in a postorder traversal because each parent
    // has to write the offset of all of its children, which we don't know in advance
    WritePageTree(hierarchy_->seen_pages_[VoxelKey::BaseKey()]);

    WriteHeader(head14);

    open_ = false;
}

// Writes the LAS header and VLRs
void WriterInternal::WriteHeader(las::LasHeader &header)
{
    laz_vlr lazVlr(header.point_format_id, header.EbByteSize(), VARIABLE_CHUNK_SIZE);

    header.vlr_count = 4; // copc_info + copc_extent + laz + wkt

    header.point_offset = OFFSET_TO_POINT_DATA;
    header.point_count = point_count_;

    if (header.EbByteSize())
    {
        header.vlr_count++;
    }

    // Set the WKT bit.
    header.global_encoding |= (1 << 4);

    out_stream_.seekp(0);

    // Convert back to lazperf header for writing
    auto laz_header = header.ToLazPerf();
    laz_header.point_format_id |= (1 << 7); // Do the lazperf trick

    laz_header.write(out_stream_);

    // Write the COPC Info VLR.
    auto copc_info_vlr = file_->GetCopcInfo().ToLazperfVlr();
    copc_info_vlr.header().write(out_stream_);
    copc_info_vlr.write(out_stream_);

    // Write the COPC Extents VLR.
    auto extents_vlr = file_->GetCopcExtents().ToCopcExtentsVlr();
    extents_vlr.header().write(out_stream_);
    extents_vlr.write(out_stream_);

    // Write the LAZ VLR
    lazVlr.header().write(out_stream_);
    lazVlr.write(out_stream_);

    // Write WKT VLR
    lazperf::wkt_vlr wkt_vlr(file_->GetWkt());
    wkt_vlr.header().write(out_stream_);
    wkt_vlr.write(out_stream_);

    // Write optional Extra Byte VLR
    if (header.EbByteSize())
    {
        auto ebVlr = this->file_->GetExtraBytes();
        ebVlr.header().write(out_stream_);
        ebVlr.write(out_stream_);
    }

    // Make sure that we haven't gone over allocated size
    if (static_cast<int64_t>(out_stream_.tellp()) > OFFSET_TO_POINT_DATA)
        throw std::runtime_error("WriterInternal::WriteHeader: LasHeader + VLRs are bigger than offset to point data.");
}

void WriterInternal::WriteChunkTable()
{
    // move to the end of the file to start emitting our compressed table
    out_stream_.seekp(0, std::ios::end);

    // take note of where we're writing the chunk table, we need this later
    auto chunk_table_offset = static_cast<int64_t>(out_stream_.tellp());

    // Fixup the chunk table to be relative offsets rather than absolute ones.
    uint64_t prevOffset = FIRST_CHUNK_OFFSET();
    for (auto &c : chunks_)
    {
        uint64_t relOffset = c.offset - prevOffset;
        prevOffset = c.offset;
        c.offset = relOffset;
    }

    // write out the chunk table header (version and total chunks)
    uint32_t version = 0;
    out_stream_.write((const char *)&version, sizeof(uint32_t));
    if (chunks_.size() > (std::numeric_limits<uint32_t>::max)())
        throw std::runtime_error("You've got way too many chunks!");
    auto numChunks = static_cast<uint32_t>(chunks_.size());
    out_stream_.write((const char *)&numChunks, sizeof(uint32_t));

    // Write the chunk table
    OutFileStream w(out_stream_);

    compress_chunk_table(w.cb(), chunks_, true);
    // go back to where we're supposed to write chunk table offset
    out_stream_.seekp(OFFSET_TO_POINT_DATA);
    out_stream_.write(reinterpret_cast<char *>(&chunk_table_offset), sizeof(chunk_table_offset));
}

// Writes a node and returns the node's offset and size in the file
Entry WriterInternal::WriteNode(std::vector<char> in, int32_t point_count, bool compressed)
{
    Entry entry;
    auto startpos = out_stream_.tellp();
    if (startpos <= 0)
        throw std::runtime_error("Error during writing node!");
    entry.offset = static_cast<uint64_t>(startpos);

    if (compressed)
        out_stream_.write(in.data(), in.size());
    else
        point_count = laz::Compressor::CompressBytes(out_stream_, file_->GetLasHeader(), in);

    point_count_ += point_count;

    auto endpos = out_stream_.tellp();
    if (endpos <= 0)
        throw std::runtime_error("Error during writing node!");

    chunks_.push_back(lazperf::chunk{static_cast<uint64_t>(point_count), static_cast<uint64_t>(endpos)});

    auto size = endpos - startpos;
    if (size > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("Chunk is too large!");
    entry.byte_size = static_cast<int32_t>(size);
    if (point_count > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("Chunk has too many points!");
    entry.point_count = point_count;
    return entry;
}

void WriterInternal::WritePage(const std::shared_ptr<PageInternal> &page)
{
    auto page_size = page->nodes.size() * 32;
    page_size += page->sub_pages.size() * 32;

    evlr_header h{0, "copc", 1000, page_size, page->key.ToString()};

    out_stream_.seekp(0, std::ios::end);
    h.write(out_stream_);

    // Set the page's offset/size
    auto offset = static_cast<uint64_t>(out_stream_.tellp());
    page->offset = offset;
    if (page_size > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("Page is too large!");
    page->byte_size = static_cast<int32_t>(page_size);

    // Set the copc header info if needed
    if (page->key == VoxelKey::BaseKey())
    {
        file_->SetCopcHier(offset, page_size);
    }

    for (const auto &node : page->nodes)
        node->Pack(out_stream_);
    for (const auto &node : page->sub_pages)
        node->Pack(out_stream_);
}

// https://en.wikipedia.org/wiki/Tree_traversal#Arbitrary_trees
void WriterInternal::WritePageTree(const std::shared_ptr<PageInternal> &current)
{
    // If the current node is empty then return.
    if (current == nullptr)
        return;

    // For each i from 1 to the current node's number of subtrees, do:
    for (const auto &child : current->sub_pages)
    {
        WritePageTree(child);
    }

    // Visit the current node for post-order traversal.
    WritePage(current);
}
} // namespace copc::Internal
