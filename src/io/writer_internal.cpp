#include <cstring>

#include <copc-lib/io/internal/writer_internal.hpp>
#include <copc-lib/laz/compressor.hpp>

#include <lazperf/filestream.hpp>
#include <lazperf/lazperf.hpp>

using namespace lazperf;

namespace copc::Internal
{

WriterInternal::WriterInternal(std::ostream &out_stream, const std::shared_ptr<CopcFile> &file,
                               std::shared_ptr<Hierarchy> hierarchy)
    : out_stream_(out_stream), file_(file), hierarchy_(hierarchy)
{
    // are extra bytes allowed to be an EVLR? If so we should just move it to be an EVLR
    // but I don't know...
    size_t eb_offset = file_->GetExtraBytes().size();
    OFFSET_TO_POINT_DATA += eb_offset + las::VlrHeader().Size;
    // reserve enough space for the header & VLRs in the file
    char out_arr[FIRST_CHUNK_OFFSET()];
    std::memset(out_arr, 0, sizeof(out_arr));
    out_stream_.write(out_arr, sizeof(out_arr));
    open_ = true;
}

void WriterInternal::Close()
{
    if (!open_)
        return;

    auto head14 = file_->GetLasHeader();
    WriteChunkTable();

    // Always write the wkt for now, since it sets evlr_offset
    // if (!file_->GetWkt().empty())
    WriteWkt(head14);

    // Page writing must be done in a postorder traversal because each parent
    // has to write the offset of all of its children, which we don't know in advance
    WritePageTree(hierarchy_->seen_pages_[VoxelKey::BaseKey()]);
    head14.evlr_count += hierarchy_->seen_pages_.size();

    WriteHeader(head14);

    open_ = false;
}

// Writes the LAS header and VLRs
void WriterInternal::WriteHeader(las::LasHeader &head14)
{
    laz_vlr lazVlr(head14.point_format_id, head14.NumExtraBytes(), VARIABLE_CHUNK_SIZE);

    // point_format_id and point_record_length  are set on open().
    head14.header_size = head14.size;
    head14.vlr_count = 2; // copc + laz

    head14.point_format_id |= (1 << 7);
    head14.point_offset = OFFSET_TO_POINT_DATA;
    head14.point_count_14 = point_count_14_;

    if (head14.NumExtraBytes())
    {
        head14.vlr_count++;
    }

    if (head14.point_count_14 > (std::numeric_limits<uint32_t>::max)())
        head14.point_count = 0;
    else
        head14.point_count = (uint32_t)head14.point_count_14;
    // Set the WKT bit.
    head14.global_encoding |= (1 << 4);

    out_stream_.seekp(0);

    // Convert back to lazperf header for writing
    auto laz_header = head14.ToLazPerf();
    laz_header.write(out_stream_);

    // Write the VLR.
    copc_data_.span = file_->GetCopc().span;
    copc_data_.header().write(out_stream_);
    copc_data_.write(out_stream_);

    lazVlr.header().write(out_stream_);
    lazVlr.write(out_stream_);

    if (head14.NumExtraBytes())
    {
        auto ebVlr = this->file_->GetExtraBytes();
        ebVlr.header().write(out_stream_);
        ebVlr.write(out_stream_);
    }
}

void WriterInternal::WriteWkt(las::LasHeader &head14)
{
    auto wkt = file_->GetWkt();
    evlr_header h{0, "LASF_Projection", 2112, (uint64_t)wkt.size(), ""};

    wkt_vlr vlr(wkt);

    out_stream_.seekp(0, std::ios::end);
    auto offset = static_cast<uint64_t>(out_stream_.tellp());
    copc_data_.wkt_vlr_offset = offset + evlr_header::Size;
    copc_data_.wkt_vlr_size = vlr.size();

    h.write(out_stream_);
    vlr.write(out_stream_);

    // TODO: Let the Page writer set the evlr offset if no wkt exists
    head14.evlr_offset = offset;
    head14.evlr_count++;
}

void WriterInternal::WriteChunkTable()
{
    // move to the end of the file to start emitting our compresed table
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

    point_count_14_ += point_count;

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

    evlr_header h{0, "entwine", 1000, page_size, page->key.ToString()};

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
        copc_data_.root_hier_offset = offset;
        copc_data_.root_hier_size = page_size;
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
