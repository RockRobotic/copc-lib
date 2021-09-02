#include <copc-lib/hierarchy/page.hpp>

namespace copc
{

bool operator==(const Page &lhs, const Page &rhs) { return lhs.IsEqual(rhs) && lhs.loaded == rhs.loaded; }
// bool Page::InsertNode(std::shared_ptr<io::Writer> writer_, VoxelKey key, std::vector<las::Point> points)
//{
//    std::vector<char> uncompressed = Node::PackPoints(points);
//    auto entry = writer_->WriteNode(uncompressed);
//    auto node = std::make_shared<Node>(key, entry.offset, entry.size, entry.point_count);
//    this->nodes[key] = node;
//}

// std::shared_ptr<Page> Page::GetSubPage(VoxelKey key) { return std::shared_ptr<Page>(); }
//
// std::shared_ptr<Node> Page::GetNode(VoxelKey key) { return std::shared_ptr<Node>(); }

} // namespace copc