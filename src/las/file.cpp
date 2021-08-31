#include <copc-lib/las/file.hpp>

namespace copc::las
{

LasFile::LasFile(LasHeader header)
{
    this->header_ = header;
}
} // namespace copc::las