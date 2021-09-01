#ifndef COPCLIB_LAS_FILE_H_
#define COPCLIB_LAS_FILE_H_

#include <iostream>
#include <map>

#include <copc-lib/las/header.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc::las
{
class LasFile
{
  public:
    LasFile(LasHeader header);

    std::vector<Vlr> vlrs;
    LasHeader GetLasHeader() const { return header_; }

  protected:
    LasHeader header_;
};
} // namespace copc::las
#endif // COPCLIB_LAS_FILE_H_