#include "copc-lib/las/vlr.hpp"
#include "copc-lib/las/utils.hpp"

namespace copc::las
{

uint8_t EXTRA_BYTE_DATA_TYPE[31]{0, 1,  1,  2, 2,  4, 4, 8, 8, 4,  8,  2,  2,  4,  4, 8,
                                 8, 16, 16, 8, 16, 3, 3, 6, 6, 12, 12, 24, 24, 12, 24};

int NumBytesFromExtraBytes(const std::vector<EbVlr::ebfield> &items)
{
    int out = 0;
    for (const auto &item : items)
    {
        if (item.data_type == 0)
            out += item.options;
        else
            out += EXTRA_BYTE_DATA_TYPE[item.data_type];
    }
    return out;
}

VlrHeader::VlrHeader(const lazperf::vlr_header &vlr_header) : evlr_flag(false)
{
    reserved = vlr_header.reserved;
    user_id = vlr_header.user_id;
    record_id = vlr_header.record_id;
    data_length = vlr_header.data_length;
    description = vlr_header.description;
}

VlrHeader::VlrHeader(const VlrHeader &vlr_header) : evlr_header(vlr_header) { evlr_flag = vlr_header.evlr_flag; }

lazperf::vlr_header VlrHeader::ToLazperfVlrHeader() const
{
    lazperf::vlr_header header;

    header.reserved = reserved;
    header.user_id = user_id;
    header.record_id = record_id;
    header.data_length = data_length;
    header.description = description;

    return header;
}
lazperf::evlr_header VlrHeader::ToLazperfEvlrHeader() const
{
    lazperf::evlr_header header;

    header.reserved = reserved;
    header.user_id = user_id;
    header.record_id = record_id;
    header.data_length = data_length;
    header.description = description;

    return header;
}

// COPC Extents VLR

CopcExtentsVlr::CopcExtentsVlr() = default;

CopcExtentsVlr::CopcExtentsVlr(int numExtents) : items(numExtents) {}

void CopcExtentsVlr::addItem(const CopcExtent &item) { items.push_back(item); }

void CopcExtentsVlr::setItem(int i, const CopcExtent &item) { items[i] = item; }

CopcExtentsVlr::~CopcExtentsVlr() = default;

CopcExtentsVlr CopcExtentsVlr::create(std::istream &in, int byteSize)
{
    CopcExtentsVlr extentsVlr;
    extentsVlr.read(in, byteSize);
    return extentsVlr;
}

void CopcExtentsVlr::read(std::istream &in, int byteSize)
{
    int numItems = static_cast<int>(byteSize / (sizeof(double) * 2));
    items.clear();

    double minimum;
    double maximum;

    for (int i = 0; i < numItems; ++i)
    {
        in.read(reinterpret_cast<char *>(&minimum), sizeof(double));
        in.read(reinterpret_cast<char *>(&maximum), sizeof(double));
        items.emplace_back(minimum, maximum);
    }
}

void CopcExtentsVlr::write(std::ostream &out) const
{
    for (const auto &i : items)
    {
        pack(i.minimum, out);
        pack(i.maximum, out);
    }
}

uint64_t CopcExtentsVlr::size() const { return items.size() * sizeof(double) * 2; }

lazperf::vlr_header CopcExtentsVlr::header() const
{
    return lazperf::vlr_header{0, "copc", 10000, (uint16_t)size(), "COPC extents"};
}

lazperf::evlr_header CopcExtentsVlr::eheader() const
{
    return lazperf::evlr_header{0, "copc", 10000, size(), "COPC extents"};
}
} // namespace copc::las
