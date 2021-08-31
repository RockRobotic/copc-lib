#include <cstdint>
#include <cstring>
#include <fstream>

#include <copc-lib/io/writer.hpp>

using namespace copc::io;
using namespace std;

int main()
{
    fstream out_stream;
    out_stream.open("test/data/test-header.laz", ios::out | ios::binary);

    Writer::LasConfig cfg;
    cfg.file_source_id = 200;
    cfg.point_format_id = 8;
    cfg.scale = vector3{2, 3, 4};
    cfg.offset = vector3{-0.02, -0.03, -40.8};
    Writer writer(out_stream, cfg, 256, "TEST_WKT!");

    writer.Close();
}