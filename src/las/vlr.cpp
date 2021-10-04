#include "copc-lib/las/vlr.hpp"

namespace copc::las
{
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
