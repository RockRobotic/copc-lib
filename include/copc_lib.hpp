#ifndef COPCLIB_COPC_COPC_H_
#define COPCLIB_COPC_COPC_H_

#include "hierarchy.hpp"
#include "reader.hpp"
#include "structs.hpp"
#include "types.hpp"

#include <utility>

namespace copc
{

class Copc
{
  public:
    Copc(const std::string &file_path);
    Copc(istream &istream);

    // Returns a header object from lazperf
    io::LasHeader GetHeader() { return this->reader_->GetHeader(); }

    // Gets the COPC header VLR data
    CopcData GetCopcData() { return this->reader_->GetCopcHeader(); }
    // Gets the WKT string
    std::string GetWktData() { return this->reader_->GetWkt(); }

    // STATIC FUNCTIONS

    // Returns the direct parent of a key within the octree
    static VoxelKey GetOctreeParent(VoxelKey key);
    // Returns all parents of a key within the octree, optionally including
    // the key itself.
    static std::vector<VoxelKey> GetOctreeParents(VoxelKey key, bool include_current = false);

    // OBJECT FUNCTIONS

    // Check if a key exists within the hierarchy of the COPC file
    bool DoesKeyExist(int d, int x, int y, int z);
    bool DoesKeyExist(VoxelKey key);

    // Returns the Entry object defining the offset, size, and point count of the
    // node at the given key.
    // If you aren't sure the key exists, call DoesKeyExist before this method.
    Entry GetEntryFromKey(int d, int x, int y, int z);
    Entry GetEntryFromKey(VoxelKey key);

    // Fetches and decompresses the given node into a char array.
    std::vector<char> ReadNodeData(Entry entry);
    // Fetches and decompresses the given node into a char array.
    // If the key doesn't exists within the hierarchy, the vector will be empty.
    std::vector<char> ReadNodeData(VoxelKey key);

    // Given an entry, this function calculates the total decompressed
    // size of that entry in bytes.
    uint64_t GetNodeSize(Entry entry);

    // TODO[LEO]
    // void PreloadHierarchyToDepth(int depth);

  private:
    shared_ptr<io::CopcReader> reader_;
    unique_ptr<Hierarchy> hierarchy_;
    unique_ptr<fstream> in_stream;

    void Initialize(istream &in_stream);
};
} // namespace copc

#endif // COPCLIB_COPC_COPC_H_