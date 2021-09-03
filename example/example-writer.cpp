//#include <cstdint>
//#include <cstring>
//#include <fstream>
//
//#include <copc-lib/io/reader.hpp>
//#include <copc-lib/io/writer.hpp>
//
// using namespace copc;
// using namespace std;
//
// void test_header()
//{
//    fstream out_stream;
//    out_stream.open("test/data/test-header.laz", ios::out | ios::binary);
//
//    Writer::LasConfig cfg;
//    cfg.file_source_id = 200;
//    cfg.point_format_id = 8;
//    cfg.scale = vector3{2, 3, 4};
//    cfg.offset = vector3{-0.02, -0.03, -40.8};
//    Writer writer(out_stream, cfg, 256, "TEST_WKT!");
//
//    writer.Close();
//}
//
// void test_page()
//{
//    fstream out_stream;
//    out_stream.open("test/data/test-page.laz", ios::out | ios::binary);
//
//    Writer::LasConfig cfg;
//    cfg.file_source_id = 200;
//    cfg.point_format_id = 8;
//    cfg.scale = vector3{2, 3, 4};
//    cfg.offset = vector3{-0.02, -0.03, -40.8};
//
//    fstream in_stream;
//    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
//    Reader reader(in_stream);
//
//    Writer writer(out_stream, cfg, 256, "TEST_WKT!");
//
//    auto page = writer.AddPage(VoxelKey(0, 0, 0, 0));
//    auto in_pts = reader.GetPoints(reader.FindNode(page.key));
//
//    writer.AddNode(page, VoxelKey(0, 0, 0, 0), in_pts);
//
//    writer.Close();
//}
//
// int main()
//{
//    // We'll get our point data from this file
//    fstream in_stream;
//    in_stream.open("test/data/autzen-classified.copc.laz", ios::in | ios::binary);
//    Reader reader(in_stream);
//    auto old_header = reader.file->GetLasHeader();
//
//    // We'll write out into this file
//    fstream out_stream;
//    out_stream.open("test/data/autzen-trimmed.copc.laz", ios::out | ios::binary);
//
//    // We need to define some LAZ attributes before creating the file
//    Writer::LasConfig cfg;
//    cfg.point_format_id = old_header.point_format_id;
//    cfg.scale = old_header.scale;
//    cfg.offset = old_header.offset;
//
//    // Now, we can create our actual writer, with an optional `span` and `wkt`:
//    Writer writer(out_stream, cfg, 256, "TEST_WKT!");
//
//    // The root page is automatically created and added for us
//    Page root_page = writer.GetRootPage();
//
//    // Let's grab the root node data from the original file:
//    std::
//
//    auto page = writer.AddPage(VoxelKey(0, 0, 0, 0));
//    auto in_pts = reader.GetPoints(reader.FindNode(page.key));
//
//    writer.AddNode(page, VoxelKey(0, 0, 0, 0), in_pts);
//
//    writer.Close();
//}
