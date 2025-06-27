#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/point_generators_2.h>

#include <chrono>

#include <torch/torch.h>
#include <torch/script.h>

#include "EdgeData.h"
#include "PointData.h"
#include "MeshFileReader.h"
#include "SimpleVoronoi2D.h"

void GenerateVoronoiDiagram(const std::map<int, PointData>& points)
{
    SimpleVoronoi2D::GenerateVoronoi2D(points);
}

void FindBoundaryEdges(const std::string& filename, std::set<EdgeData>& outEdges)
{
    std::vector<EdgeData> allEdges;
    MeshFileReader::ExtractEdgesFromMesh(filename, allEdges);

    for (int i = 0; i < allEdges.size(); ++i)
    {
        // For 2D, 1 edge can at most be shared by 2 faces
        EdgeData e =  allEdges[i];
        if (outEdges.find(e) == outEdges.end()) 
        {
            outEdges.insert(e);
        } 
        else 
        {
            outEdges.erase(e);
        }
    }

    for(const auto& e : outEdges)
    {
        std::cout << e.mStart << ", " << e.mEnd << std::endl;
    }
}

void GetPointsFromBoundaryEdges(std::set<EdgeData>& edges, std::map<int, PointData> outPoint)
{
    int id = 1;
    for(const auto& e : edges)
    {
        outPoint.insert(std::make_pair(id, PointData(id, e.mStart, e.mEnd)));
        ++id;
    }
}

int main(int argc, char* argv[])
{
    std::set<EdgeData> edges;

    FindBoundaryEdges("./data/SkeletonizationTrainingData/rectangle.msh", edges);

    std::map<int, PointData> points;
    GetPointsFromBoundaryEdges(edges, points);

    GenerateVoronoiDiagram(points);

    return 0;
}