#ifndef COPCLIB_IO_BASE_H_
#define COPCLIB_IO_BASE_H_

#include <copc-lib/copc/file.hpp>
#include <copc-lib/hierarchy/internal/hierarchy.hpp>
#include <copc-lib/hierarchy/node.hpp>
#include <copc-lib/hierarchy/page.hpp>
#include <copc-lib/las/vlr.hpp>

namespace copc
{
// Base class for all IO reader/writers
class BaseIO
{
  public:
    // Find a node object given a key
    Node FindNode(VoxelKey key);

    // WKT string if defined, else empty
    std::string GetWkt() const { return this->file->GetWkt(); }
    // CopcData
    las::CopcVlr GetCopcHeader() const { return this->file->GetCopc(); }
    // Las header
    las::LasHeader GetLasHeader() const { return this->file->GetLasHeader(); }
    // EB Vlr
    las::EbVlr GetExtraByteVlr() const { return this->file->GetExtraBytes(); }

  protected:
    std::shared_ptr<CopcFile> file;
    std::shared_ptr<Internal::Hierarchy> hierarchy;
    virtual std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) = 0;
    void ReadAndParsePage(std::shared_ptr<Internal::PageInternal> page);
    // Recursively reads all subpages and nodes given a root and returns all the nodes that were loaded
    void LoadPageHierarchy(std::shared_ptr<Internal::PageInternal> root, std::vector<Node> &loaded_nodes);
};
} // namespace copc
#endif // COPCLIB_IO_BASE_H_
