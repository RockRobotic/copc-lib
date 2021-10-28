#include "copc-lib/las/vlr.hpp"

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

} // namespace copc::las
