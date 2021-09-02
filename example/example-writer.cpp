#include <cstdint>
#include <cstring>
#include <fstream>

#include <copc-lib/io/reader.hpp>
#include <copc-lib/io/writer.hpp>

using namespace copc;
using namespace std;

void test_header()
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

void test_page()
{
    fstream out_stream;
    out_stream.open("test/data/test-page.laz", ios::out | ios::binary);

    Writer::LasConfig cfg;
    cfg.file_source_id = 200;
    cfg.point_format_id = 8;
    cfg.scale = vector3{2, 3, 4};
    cfg.offset = vector3{-0.02, -0.03, -40.8};

    fstream in_stream;
    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
    Reader reader(in_stream);

    Writer writer(out_stream, cfg, 256, "TEST_WKT!");

    auto page = writer.CreateAndInsertPage(VoxelKey(0, 0, 0, 0));
    auto in_pts = reader.GetPoints(reader.FindNode(page.key));

    writer.InsertNode(page, VoxelKey(0, 0, 0, 0), in_pts);

    writer.Close();
}

int main()
{
    test_header();
    test_page();
}