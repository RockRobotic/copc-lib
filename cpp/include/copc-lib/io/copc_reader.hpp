#ifndef COPCLIB_IO_COPC_READER_H_
#define COPCLIB_IO_COPC_READER_H_

#include <istream>
#include <limits>
#include <map>
#include <string>

#include "copc-lib/copc/copc_config.hpp"
#include "copc-lib/hierarchy/key.hpp"
#include "copc-lib/io/base_reader.hpp"
#include "copc-lib/io/copc_base_io.hpp"
#include "copc-lib/las/points.hpp"
#include "copc-lib/las/vlr.hpp"

namespace copc
{
namespace Internal
{
class PageInternal;
} // namespace Internal

class Reader : public BaseIO, public BaseReader
{
  public:
    Reader(std::istream *in_stream) : BaseReader(in_stream) { InitCopcReader(); }

    // Reads the node's data into an uncompressed byte array
    // Node needs to be valid for this function, it will error
    std::vector<char> GetPointData(Node const &node);
    // VoxelKey can be invalid, function will return empty arr
    std::vector<char> GetPointData(VoxelKey const &key);
    // Reads the node's data into Point objects
    las::Points GetPoints(Node const &node);
    las::Points GetPoints(VoxelKey const &key);
    // Reads node data without decompressing
    std::vector<char> GetPointDataCompressed(Node const &node);
    std::vector<char> GetPointDataCompressed(VoxelKey const &key);

    // Return all children of a page with a given key
    // (or the node itself, if it exists, if there isn't a page with that key)
    std::vector<Node> GetAllChildrenOfPage(const VoxelKey &key);
    // Helper function to get all nodes from the root
    std::vector<Node> GetAllNodes() { return GetAllChildrenOfPage(VoxelKey::RootKey()); }

    // Return all keys of pages in copc hierarchy
    std::vector<VoxelKey> GetPageList();

    // Helper function to get all points from the root
    las::Points GetAllPoints(double resolution = 0);

    // Resolution query functions
    // The resulting resolution may not be exactly this value: the minimum possible resolution that is at least as
    // precise as the requested resolution will be selected. Therefore the result may be a bit more precise than
    // requested.
    int32_t GetDepthAtResolution(double resolution);
    int32_t GetMaxDepth();
    std::vector<Node> GetNodesAtResolution(double resolution);
    std::vector<Node> GetNodesWithinResolution(double resolution);

    // Spatial query functions
    // Definitions taken from https://shapely.readthedocs.io/en/stable/manual.html#binary-predicates
    std::vector<Node> GetNodesWithinBox(const Box &box, double resolution = 0);
    std::vector<Node> GetNodesIntersectBox(const Box &box, double resolution = 0);
    las::Points GetPointsWithinBox(const Box &box, double resolution = 0);
    bool ValidateSpatialBounds(bool verbose = false);
    // TODO: Add a function to validate extents.

    copc::CopcConfig CopcConfig() { return config_; }

  protected:
    Reader() = default;
    void InitCopcReader();
    copc::CopcConfig config_;

    // Finds and loads the COPC vlr
    CopcInfo ReadCopcInfoVlr(std::map<uint64_t, las::VlrHeader> &vlrs);
    // Finds and loads the COPC vlr
    CopcExtents ReadCopcExtentsVlr(std::map<uint64_t, las::VlrHeader> &vlrs, const las::EbVlr &eb_vlr) const;

    std::vector<Entry> ReadPage(std::shared_ptr<Internal::PageInternal> page) override;
};

class FileReader : public Reader
{
  public:
    FileReader(const std::string &file_path) : is_open_(true)
    {
        auto f_stream = new std::fstream;
        this->file_path_ = file_path;
        f_stream->open(file_path.c_str(), std::ios::in | std::ios::binary);
        if (!f_stream->good())
            throw std::runtime_error("FileReader: Error while opening file path.");
        in_stream_ = f_stream;

        InitReader();
        InitCopcReader();
    }

    void Close()
    {
        if (is_open_)
        {
            dynamic_cast<std::fstream *>(in_stream_)->close();
            delete in_stream_;
            is_open_ = false;
        }
    }

    std::string FilePath() { return file_path_; }

    ~FileReader() { Close(); }

  private:
    bool is_open_;
    std::string file_path_;
};

} // namespace copc
#endif // COPCLIB_IO_COPC_READER_H_
