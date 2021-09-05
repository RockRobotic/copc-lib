#include <cstring>

#include <copc-lib/io/internal/writer_internal.hpp>
#include <copc-lib/laz/compressor.hpp>

#include <lazperf/filestream.hpp>
#include <lazperf/lazperf.hpp>

using namespace lazperf;

namespace copc::Internal
{

WriterInternal::WriterInternal(std::ostream &out_stream, std::shared_ptr<CopcFile> file,
                               std::shared_ptr<Hierarchy> hierarchy)
    : out_stream(out_stream), file(file), hierarchy(hierarchy)
{
    OFFSET_TO_POINT_DATA += file->GetExtraBytes().size() + las::VlrHeader().Size;
    // reserve enough space for the header & VLRs in the file
    char out_arr[FIRST_CHUNK_OFFSET()];
    std::memset(out_arr, 0, sizeof(out_arr));
    out_stream.write(out_arr, sizeof(out_arr));
    open = true;
}

void WriterInternal::Close()
{
    if (!open)
        return;

    auto head14 = file->GetLasHeader();
    WriteChunkTable();

    // Always write the wkt for now, since it sets evlr_offset
    // if (!file->GetWkt().empty())
    WriteWkt(head14);

    // Page writing must be done in a postorder traversal because each parent
    // has to write the offset of all of its children, which we don't know in advance
    WritePageTree(hierarchy->seen_pages_[VoxelKey::BaseKey()]);
    head14.evlr_count += hierarchy->seen_pages_.size();

    WriteHeader(head14);

    open = false;
}

// Writes the LAS header and VLRs
void WriterInternal::WriteHeader(las::LasHeader &head14)
{
    laz_vlr lazVlr(head14.pointFormat(), head14.ebCount(), VARIABLE_CHUNK_SIZE);

    // point_format_id and point_record_length  are set on open().
    head14.header_size = head14.sizeFromVersion();
    head14.vlr_count = 2; // copc + laz

    head14.point_format_id |= (1 << 7);
    head14.point_offset = OFFSET_TO_POINT_DATA;
    head14.point_count_14 = point_count_14_;

    if (head14.ebCount())
    {
        head14.vlr_count++;
    }

    if (head14.point_count_14 > (std::numeric_limits<uint32_t>::max)())
        head14.point_count = 0;
    else
        head14.point_count = (uint32_t)head14.point_count_14;
    // Set the WKT bit.
    head14.global_encoding |= (1 << 4);

    out_stream.seekp(0);
    head14.write(out_stream);

    // Write the VLR.
    copc_data.span = file->GetCopc().span;
    copc_data.header().write(out_stream);
    copc_data.write(out_stream);

    lazVlr.header().write(out_stream);
    lazVlr.write(out_stream);

    if (head14.ebCount())
    {
        auto ebVlr = this->file->GetExtraBytes();
        ebVlr.header().write(out_stream);
        ebVlr.write(out_stream);
    }
}

void WriterInternal::WriteWkt(las::LasHeader &head14)
{
    auto wkt = file->GetWkt();
    evlr_header h{0, "LASF_Projection", 2112, (uint64_t)wkt.size(), ""};

    wkt_vlr vlr(wkt);

    out_stream.seekp(0, std::ios::end);
    int64_t offset = static_cast<int64_t>(out_stream.tellp());
    copc_data.wkt_vlr_offset = offset + evlr_header::Size;
    copc_data.wkt_vlr_size = vlr.size();

    h.write(out_stream);
    vlr.write(out_stream);

    // TODO: Let the Page writer set the evlr offset if no wkt exists
    head14.evlr_offset = offset;
    head14.evlr_count++;
}

void WriterInternal::WriteChunkTable()
{
    // move to the end of the file to start emitting our compresed table
    out_stream.seekp(0, std::ios::end);

    // take note of where we're writing the chunk table, we need this later
    int64_t chunk_table_offset = static_cast<int64_t>(out_stream.tellp());

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

// Writes a node and returns the node's offset and size in the file
Entry WriterInternal::WriteNode(std::vector<char> in, uint64_t point_count, bool compressed)
{
    Entry entry;
    entry.offset = out_stream.tellp();

    if (compressed)
        out_stream.write(in.data(), in.size());
    else
        point_count = laz::Compressor::CompressBytes(out_stream, file->GetLasHeader(), in);

    point_count_14_ += point_count;

    uint64_t endpos = (uint64_t)out_stream.tellp();
    chunks_.push_back(lazperf::chunk{point_count, endpos});

    entry.size = endpos - entry.offset;
    entry.point_count = point_count;
    return entry;
}

void WriterInternal::WritePage(std::shared_ptr<PageInternal> page)
{
    uint64_t page_size = page->nodes.size() * 32;
    page_size += page->sub_pages.size() * 32;

    evlr_header h{0, "entwine", 1000, page_size, page->key.ToString()};

    out_stream.seekp(0, std::ios::end);
    h.write(out_stream);

    // Set the page's offset/size
    int64_t offset = static_cast<int64_t>(out_stream.tellp());
    page->offset = offset;
    page->size = page_size;

    // Set the copc header info if needed
    if (page->key == VoxelKey::BaseKey())
    {
        copc_data.root_hier_offset = offset;
        copc_data.root_hier_size = page_size;
    }

    for (auto node : page->nodes)
        node->Pack(out_stream);
    for (auto node : page->sub_pages)
        node->Pack(out_stream);
}

// https://en.wikipedia.org/wiki/Tree_traversal#Arbitrary_trees
void WriterInternal::WritePageTree(std::shared_ptr<PageInternal> current)
{
    // If the current node is empty then return.
    if (current == nullptr)
        return;

    // For each i from 1 to the current node's number of subtrees, do:
    for (auto child : current->sub_pages)
    {
        WritePageTree(child);
    }

    // Visit the current node for post-order traversal.
    WritePage(current);
}
} // namespace copc::Internal
