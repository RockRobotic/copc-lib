#include <cstring>
#include <iterator>
#include <stdexcept>

#include "copc-lib/copc/extents.hpp"
#include "copc-lib/hierarchy/internal/hierarchy.hpp"
#include "copc-lib/io/internal/copc_writer_internal.hpp"
#include "copc-lib/laz/compressor.hpp"

#include <lazperf/lazperf.hpp>
#include <lazperf/vlr.hpp>

namespace copc::Internal
{

size_t WriterInternal::OffsetToPointData() const
{
    size_t base_offset(las::LAZ_HEADER_SIZE + (54 + (34 + 4 * 6))); // header + LAZ vlr (max 4 items)
    size_t eb_offset = config_->ExtraBytesVlr().size();
    if (eb_offset > 0)
        eb_offset += lazperf::vlr_header::Size;

    size_t laz_offset = base_offset + eb_offset;

    size_t copc_header_offset = (54 + 160); // COPC extra header size

    size_t extents_offset =
        CopcExtents::ByteSize(config_->LasHeader()->PointFormatId(), config_->ExtraBytesVlr().items.size()) +
        lazperf::vlr_header::Size;
    // If we store extended stats we need two extents VLRs
    if (config_->CopcExtents()->HasExtendedStats())
        extents_offset *= 2;

    return laz_offset + copc_header_offset + extents_offset;
}

WriterInternal::WriterInternal(std::ostream &out_stream, std::shared_ptr<CopcConfigWriter> copc_config,
                               std::shared_ptr<Hierarchy> hierarchy)
    : BaseWriter(out_stream), config_(copc_config), hierarchy_(hierarchy)
{
    // reserve enough space for the header & VLRs in the file
    std::fill_n(std::ostream_iterator<char>(out_stream_), FirstChunkOffset(), 0);
    open_ = true;
}

void WriterInternal::Close()
{
    if (!open_)
        return;

    WriteChunkTable();

    // Set hierarchy evlr
    out_stream_.seekp(0, std::ios::end);
    evlr_offset_ = static_cast<int64_t>(out_stream_.tellp());
    evlr_count_ += hierarchy_->seen_pages_.size();

    // Compute the hierarchy between existing pages
    ComputePageHierarchy();
    // Page writing must be done in a postorder traversal because each parent
    // has to write the offset of all of its children, which we don't know in advance
    WritePageTree(hierarchy_->seen_pages_[VoxelKey::RootKey()]);

    // If WKT is provided, write it as EVLR
    if (!config_->Wkt().empty())
    {
        evlr_count_++;
        lazperf::wkt_vlr wkt_vlr(config_->Wkt());
        wkt_vlr.eheader().write(out_stream_);
        wkt_vlr.write(out_stream_);
    }

    WriteHeader();

    open_ = false;
}

// Writes the LAS header and VLRs
void WriterInternal::WriteHeader()
{
    // Write LAS header
    auto las_header_vlr =
        config_->LasHeader()->ToLazPerf(OffsetToPointData(), point_count_, evlr_offset_, evlr_count_,
                                        config_->LasHeader()->EbByteSize(), config_->CopcExtents()->HasExtendedStats());
    out_stream_.seekp(0);
    las_header_vlr.write(out_stream_);

    // Write the COPC Info VLR.
    auto copc_info_vlr = config_->CopcInfo()->ToLazPerf(*config_->CopcExtents()->GpsTime());
    copc_info_vlr.header().write(out_stream_);
    copc_info_vlr.write(out_stream_);

    // Write the COPC Extents VLR.
    auto extents_vlr = config_->CopcExtents()->ToLazPerf({las_header_vlr.minx, las_header_vlr.maxx},
                                                         {las_header_vlr.miny, las_header_vlr.maxy},
                                                         {las_header_vlr.minz, las_header_vlr.maxz});
    extents_vlr.header().write(out_stream_);
    extents_vlr.write(out_stream_);

    // Write the COPC Extended stats VLR.
    if (config_->CopcExtents()->HasExtendedStats())
    {
        auto extended_stats_vlr = config_->CopcExtents()->ToLazPerfExtended();

        auto header = extended_stats_vlr.header();
        header.user_id = "rock_robotic";
        header.record_id = 10001;
        header.description = "COPC extended stats";

        header.write(out_stream_);
        extended_stats_vlr.write(out_stream_);
    }

    // Write the LAZ VLR
    lazperf::laz_vlr lazVlr(config_->LasHeader()->PointFormatId(), config_->LasHeader()->EbByteSize(),
                            VARIABLE_CHUNK_SIZE);
    lazVlr.header().write(out_stream_);
    lazVlr.write(out_stream_);

    // Write optional Extra Byte VLR
    if (config_->LasHeader()->EbByteSize() > 0)
    {
        auto ebVlr = this->config_->ExtraBytesVlr();
        ebVlr.header().write(out_stream_);
        ebVlr.write(out_stream_);
    }

    // Make sure that we haven't gone over allocated size
    if (static_cast<int64_t>(out_stream_.tellp()) > OffsetToPointData())
        throw std::runtime_error("WriterInternal::WriteHeader: LasHeader + VLRs are bigger than offset to point data.");
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
        point_count = laz::Compressor::CompressBytes(out_stream_, *config_->LasHeader(), in);

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
    if (page->key == VoxelKey::RootKey())
    {
        config_->CopcInfo()->root_hier_offset = offset;
        config_->CopcInfo()->root_hier_size = page_size;
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
