#ifndef COPCLIB_IO_TYPES_H_
#define COPCLIB_IO_TYPES_H_

#include <lazperf/header.hpp>
#include <lazperf/vlr.hpp>

namespace copc
{

namespace io
{
using LasHeader = lazperf::header12;
using VlrHeader = lazperf::vlr_header;
using CopcVlr = lazperf::copc_vlr;
using WktVlr = lazperf::wkt_vlr;
} // namespace io

} // namespace copc

#endif // COPCLIB_IO_TYPES_H_