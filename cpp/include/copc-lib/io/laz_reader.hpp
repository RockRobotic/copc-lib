#ifndef COPCLIB_IO_LAZ_READER_H_
#define COPCLIB_IO_LAZ_READER_H_

#include "copc-lib/io/base_reader.hpp"

namespace copc::laz
{

class LazReader : public BaseReader
{
  public:
    LazReader(std::istream *in_stream) : BaseReader(in_stream) {}

    std::vector<char> GetPointData();
    las::Points GetPoints();

    las::LazConfig LazConfig() { return las_config_; }

  protected:
    LazReader() = default;
};

class LazFileReader : public LazReader
{
  public:
    LazFileReader(const std::string &file_path) : is_open_(true)
    {
        auto f_stream = new std::fstream;
        this->file_path_ = file_path;
        f_stream->open(file_path.c_str(), std::ios::in | std::ios::binary);
        if (!f_stream->good())
            throw std::runtime_error("LazFileReader: Error while opening file path.");
        in_stream_ = f_stream;

        InitReader();
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

    ~LazFileReader() { Close(); }

  private:
    bool is_open_;
    std::string file_path_;
};

} // namespace copc::laz

#endif // COPCLIB_IO_LAZ_READER_H_
