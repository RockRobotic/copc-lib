#include "copc_lib.hpp"

#include <utility>

namespace copc
{

Copc::Copc(const std::string &file_path)
{
    in_stream = make_unique<fstream>();
    in_stream->open(file_path, ios::in | ios::binary);
    if (!in_stream->good())
    {
        throw runtime_error("Can't open specified file: " + file_path);
    }

    Initialize(*in_stream);
}

Copc::Copc(istream &istream) { Initialize(istream); }

void Copc::Initialize(istream &in_stream)
{
    reader_ = make_shared<io::CopcReader>(in_stream);
    hierarchy_ = make_unique<Hierarchy>(reader_.get());
}

bool Copc::DoesKeyExist(int d, int x, int y, int z) { return DoesKeyExist(VoxelKey(d, x, y, z)); }

bool Copc::DoesKeyExist(VoxelKey key) { return hierarchy_->GetKey(key).IsValid(); }

VoxelKey Copc::GetOctreeParent(VoxelKey key) { return Hierarchy::GetParent(key); }

std::vector<VoxelKey> Copc::GetOctreeParents(VoxelKey key, bool include_current)
{
    return Hierarchy::GetParents(key, include_current);
}

Entry Copc::GetEntryFromKey(int d, int x, int y, int z) { return GetEntryFromKey(VoxelKey(d, x, y, z)); }

// If you aren't sure the key exists, call DoesKeyExist before this method.
Entry Copc::GetEntryFromKey(VoxelKey key) { return hierarchy_->GetKey(key); }

std::vector<char> Copc::ReadNodeData(Entry entry) { return reader_->GetPoints(entry); }

std::vector<char> Copc::ReadNodeData(VoxelKey key)
{
    auto entry = hierarchy_->GetKey(key);
    return reader_->GetPoints(entry);
}

uint64_t Copc::GetNodeSize(Entry entry)
{
    return entry.GetDecompressedSize(this->reader_->GetHeader().point_record_length);
}
} // namespace copc
