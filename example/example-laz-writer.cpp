#include <cassert>
#include <random>

#include <copc-lib/geometry/vector3.hpp>
#include <copc-lib/io/laz_writer.hpp>

using namespace copc;
using namespace std;

// constants
const Vector3 MIN_BOUNDS = {-2000, -5000, 20}; // Scaled coordinates
const Vector3 MAX_BOUNDS = {5000, 1034, 125};  // Scaled coordinates
const int NUM_POINTS = 3000;

// random num devices
std::random_device rd;  // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator

// This function will generate `NUM_POINTS` random points within the LAS bounds
las::Points RandomPoints(const las::LasHeader &header, int number_points)
{

    // Random num generators between the min and max spatial bounds of the las header
    std::uniform_int_distribution<> rand_x(header.RemoveScaleX(header.min.x), header.RemoveScaleX(header.max.x));
    std::uniform_int_distribution<> rand_y(header.RemoveScaleY(header.min.y), header.RemoveScaleY(header.max.y));
    std::uniform_int_distribution<> rand_z(header.RemoveScaleZ(header.min.z), header.RemoveScaleZ(header.max.z));

    // Create a Points object based on the LAS header
    las::Points points(header);

    // Populate the points
    for (int i = 0; i < number_points; i++)
    {
        // Create a point with a given point format
        // The use of las::Point constructor is strongly discouraged, instead use las::Points::CreatePoint
        auto point = points.CreatePoint();
        // point has getters/setters for all attributes
        point->X(header.ApplyScaleX(rand_x(gen)));
        point->Y(header.ApplyScaleY(rand_y(gen)));
        point->Z(header.ApplyScaleZ(rand_z(gen)));

        points.AddPoint(point);
    }
    return points;
}

// In this example, we'll create our own LAZ file from scratch
void NewFileExample()
{
    // Create our new LAZ file with the specified format, scale, offset, and wkt
    las::LazConfigWriter cfg(8, {0.1, 0.1, 0.1}, {50, 50, 50}, "TEST_WKT");
    // copc-lib will not automatically compute the min/max of added points
    // so we will have to calculate it ourselves
    cfg.LasHeader()->min = MIN_BOUNDS;
    cfg.LasHeader()->max = MAX_BOUNDS;

    // Now, we can create a LAZ writer
    laz::LazFileWriter writer("new-laz.laz", cfg);
    auto header = *writer.LazConfig()->LasHeader();

    // Let's generate some points
    auto points = RandomPoints(header, NUM_POINTS);

    // The points will be written to the file when we call WritePoints
    writer.WritePoints(points);

    // We can write a second batch of points in a different chunk
    points = RandomPoints(header, NUM_POINTS);
    writer.WritePoints(points);

    // Make sure we call close to finish writing the file!
    writer.Close();
}

int main() { NewFileExample(); }
