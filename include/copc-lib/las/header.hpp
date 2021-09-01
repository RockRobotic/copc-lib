#ifndef COPCLIB_LAS_HEADER_H_
#define COPCLIB_LAS_HEADER_H_

#include <lazperf/header.hpp>
#include <lazperf/readers.hpp>

namespace copc::las
{
using VlrHeader = lazperf::vlr_header;
// todo: create our own lasheader class so we can switch between header12/14
using LasHeader = lazperf::header14;
} // namespace copc::las
#endif // COPCLIB_LAS_HEADER_H_