#ifndef COPCLIB_IO_LAZ_READER_H_
#define COPCLIB_IO_LAZ_READER_H_

#include "copc-lib/las/laz_config.hpp"

namespace copc::laz
{

class LazReader
{
  public:
    Reader(std::istream *in_stream) : in_stream_(in_stream) { InitReader(); }

    // Write a group of points as a chunk
    void WritePoints(const las::Points &points);

    las::LazConfig LazConfig() { return *config_; }
};

class LazFileWriter : public BaseFileWriter
{

  public:
    LazFileWriter(const std::string &file_path, const las::LazConfigWriter &laz_config_writer);
    void Close() override;
    ~LazFileWriter() { Close(); };

    // Write a group of points as a chunk
    void WritePoints(const las::Points &points) { writer_->WritePoints(points); }
    uint64_t PointCount() { return writer_->PointCount(); }
    uint64_t ChunkCount() { return writer_->ChunkCount(); }
    std::shared_ptr<las::LazConfigWriter> LazConfig() { return writer_->LazConfig(); }

  private:
    std::shared_ptr<LazReader> reader_;
};

} // namespace copc::laz

#endif // COPCLIB_IO_LAZ_READER_H_
