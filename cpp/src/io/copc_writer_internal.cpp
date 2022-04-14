#include <cstring>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/io/internal/copc_writer_internal.hpp"

#include <lazperf/lazperf.hpp>
#include <lazperf/vlr.hpp>
#include <utility>

namespace copc::Internal
{

size_t WriterInternal::OffsetToPointData() const
{
    size_t base_laz_offset = laz::BaseWriter::OffsetToPointData();

    // COPC VLR
    size_t copc_info_vlr_size = (lazperf::vlr_header::Size + CopcInfo::VLR_SIZE_BYTES);

    // COPC Extents VLR
    size_t copc_extents_vlr_size =
        CopcExtents::ByteSize(GetConfig()->LasHeader()->PointFormatId(), GetConfig()->ExtraBytesVlr().items.size());
    copc_extents_vlr_size += lazperf::vlr_header::Size;
    // If we store extended stats we need two extents VLRs
    if (GetConfig()->CopcExtents()->HasExtendedStats())
        copc_extents_vlr_size *= 2;

    return base_laz_offset + copc_info_vlr_size + copc_extents_vlr_size;
}

WriterInternal::WriterInternal(std::ostream &out_stream, const std::shared_ptr<CopcConfigWriter> &copc_config_writer,
                               std::shared_ptr<Hierarchy> hierarchy)
    : BaseWriter(out_stream, std::static_pointer_cast<las::LazConfig>(copc_config_writer)),
      hierarchy_(std::move(hierarchy))
{
    // reserve enough space for the header & VLRs in the file
    std::fill_n(std::ostream_iterator<char>(out_stream_), FirstChunkOffset(), 0);
}

void WriterInternal::Close()
{
    if (!open_)
        return;

    WriteChunkTable();

    // Set COPC hierarchy evlr
    out_stream_.seekp(0, std::ios::end);
    evlr_offset_ = static_cast<int64_t>(out_stream_.tellp());
    evlr_count_ += hierarchy_->seen_pages_.size();

    // Compute the hierarchy between existing pages
    ComputePageHierarchy();
    // Page writing must be done in a postorder traversal because each parent
    // has to write the offset of all of its children, which we don't know in advance
    WritePageTree(hierarchy_->seen_pages_[VoxelKey::RootKey()]);

    WriteWKT();

    WriteHeader();

    open_ = false;
}

// Writes the LAS header and VLRs
void WriterInternal::WriteHeader()
{
    WriteLasHeader(GetConfig()->CopcExtents()->HasExtendedStats());

    // Write the COPC Info VLR.
    lazperf::copc_info_vlr copc_info_vlr = GetConfig()->CopcInfo()->ToLazPerf(*GetConfig()->CopcExtents()->GpsTime());
    copc_info_vlr.header().write(out_stream_);
    copc_info_vlr.write(out_stream_);

    // Write the COPC Extents VLR.
    auto extents_vlr =
        GetConfig()->CopcExtents()->ToLazPerf({GetConfig()->LasHeader()->min.x, GetConfig()->LasHeader()->max.x},
                                              {GetConfig()->LasHeader()->min.y, GetConfig()->LasHeader()->max.y},
                                              {GetConfig()->LasHeader()->min.z, GetConfig()->LasHeader()->max.z});
    extents_vlr.header().write(out_stream_);
    extents_vlr.write(out_stream_);

    // Write the COPC Extended stats VLR.
    if (GetConfig()->CopcExtents()->HasExtendedStats())
    {
        auto extended_stats_vlr = GetConfig()->CopcExtents()->ToLazPerfExtended();

        auto header = extended_stats_vlr.header();
        header.user_id = "rock_robotic";
        header.record_id = 10001;
        header.description = "COPC extended stats";

        header.write(out_stream_);
        extended_stats_vlr.write(out_stream_);
    }

    WriteLazAndEbVlrs();

    // Make sure that we haven't gone over allocated size
    if (static_cast<int64_t>(out_stream_.tellp()) > OffsetToPointData())
        throw std::runtime_error("WriterInternal::WriteHeader: LasHeader + VLRs are bigger than offset to point data.");
}

// Writes a node and returns the node's offset and size in the file
Entry WriterInternal::WriteNode(const std::vector<char> &in, int32_t point_count, bool compressed)
{
    Entry entry;

    entry.point_count = WriteChunk(in, point_count, compressed, &entry.offset, &entry.byte_size);

    return entry;
}

void WriterInternal::WritePage(const std::shared_ptr<PageInternal> &page)
{
    auto page_size = page->nodes.size() * 32;
    page_size += page->sub_pages.size() * 32;

    lazperf::evlr_header h{0, "copc", 1000, page_size, page->key.ToString()};

    out_stream_.seekp(0, std::ios::end);
    h.write(out_stream_);

    // Set the page's offset/size
    auto offset = static_cast<uint64_t>(out_stream_.tellp());
    page->offset = offset;
    if (page_size > (std::numeric_limits<int32_t>::max)())
        throw std::runtime_error("Page is too large!");
    page->byte_size = static_cast<int32_t>(page_size);

    // Set the copc header info if needed
    if (page->key == VoxelKey::RootKey())
    {
        GetConfig()->CopcInfo()->root_hier_offset = offset;
        GetConfig()->CopcInfo()->root_hier_size = page_size;
    }

    for (const auto &node : page->nodes)
        node.second->Pack(out_stream_);
    for (const auto &node : page->sub_pages)
        node->Pack(out_stream_);
}

void WriterInternal::ComputePageHierarchy()
{
    // loop through each page
    for (const auto &page : hierarchy_->seen_pages_)
    {
        if (page.first != VoxelKey::RootKey())
        {
            auto parent_key = page.first.GetParent();
            while (!hierarchy_->PageExists(parent_key))
            {
                parent_key = parent_key.GetParent();
                if (!parent_key.IsValid())
                    throw std::runtime_error("Can't find parent of node: " + page.first.ToString());
            }
            hierarchy_->seen_pages_[parent_key]->sub_pages.insert(page.second);
        }
    }
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
