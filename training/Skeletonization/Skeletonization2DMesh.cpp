#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/point_generators_2.h>

#include <chrono>

#include <torch/torch.h>
#include <torch/script.h>

#include "EdgeData.h"
#include "MeshFileReader.h"

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

int main(int argc, char* argv[])
{
    std::set<EdgeData> edges;

    FindBoundaryEdges("./data/SkeletonizationTrainingData/rectangle.msh", edges);

    return 0;
}