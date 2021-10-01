#include "copc-lib/hierarchy/page.hpp"

namespace copc
{

bool operator==(const Page &lhs, const Page &rhs) { return lhs.IsEqual(rhs) && lhs.loaded == rhs.loaded; }

} // namespace copc
