#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/point_generators_2.h>
#include <limits>
#include <chrono>

#include <torch/torch.h>
#include <torch/script.h>

#include "EdgeData.h"
#include "PointData.h"
#include "MeshFileReader.h"
#include "SimpleVoronoi2D.h"
#include "TrainUsingVoronoiDiagram.h"

#include "voronoidiagram.hpp"

#define EPSILON 1e-6f

bool IsColinear(const PointData& A, const PointData& B, const PointData& C) 
{
    float area = (A.mX * (B.mY - C.mY)) + (B.mX * (C.mY - A.mY)) + (C.mX * (A.mY - B.mY));
    return std::fabs(area) < EPSILON;
}

void RemoveColinearPoints(const std::vector<PointData>& input, std::vector<PointData>& outPoints) 
{
    int n = input.size();
    if (n < 3) 
    {
        return; // can't simplify further
    }

    for (int i = 0; i < n; ++i) {
        const PointData& prev = input[(i - 1 + n) % n];
        const PointData& curr = input[i];
        const PointData& next = input[(i + 1) % n];

        if (!IsColinear(prev, curr, next)) 
        {
            outPoints.push_back(curr);
        }
    }

    std::cout << "Cleaned Points After removing colinear points:" << std::endl;
    for(const auto& p : outPoints)
    {
        std::cout << p.mId << ", " << p.mX << ", " << p.mY << std::endl;
    }
}

void RemoveDuplicatePoints(const std::vector<PointData>& input, std::vector<PointData>& outPoints) 
{
    std::unordered_set<std::string> seen;

    for (const auto& pt : input) {
        // Round coordinates to fixed precision to avoid float inaccuracies
        int xKey = static_cast<int>(std::round(pt.mX / EPSILON));
        int yKey = static_cast<int>(std::round(pt.mY / EPSILON));
        std::string key = std::to_string(xKey) + "_" + std::to_string(yKey);

        if (seen.find(key) == seen.end()) 
        {
            outPoints.push_back(pt);
            seen.insert(key);
        }
    }
}

void ReorderPoints(std::map<int, PointData>& unorderedPoints, std::vector<EdgeData>& edges, std::vector<PointData>& outPoints)
{
    int nextStartId = -1;

    auto isPointAdded = [&](int id)
    {
        for(const auto& p : outPoints)
        {
            if(p.mId == id)
            {
                return true;
            }
        }

        return false;
    };

    auto findNextEdge = [&]()  
    {
        int idx = -1;
        for(int i = 0; i < edges.size(); ++i)
        {
            const EdgeData& e = edges[i];

            if(e.mAddedToMeshLoop)
            {
                continue;
            }

            if(nextStartId == e.mStart || nextStartId == e.mEnd)
            {
                idx = i;
                break;
            }
        }

        return idx;
    };

    int edgeToProcess = 0;

    while(outPoints.size() != unorderedPoints.size())
    {
        if(outPoints.begin() == outPoints.end())
        {
            EdgeData& e = edges[edgeToProcess];
            int startId = e.mStart;
            int endId = e.mEnd;
            outPoints.push_back(PointData(startId, unorderedPoints[startId].mX, unorderedPoints[startId].mY));
            outPoints.push_back(PointData(endId, unorderedPoints[endId].mX, unorderedPoints[endId].mY));
            nextStartId = endId;
            e.mAddedToMeshLoop = true;
            continue;
        }

        edgeToProcess = findNextEdge();
        
        EdgeData& e = edges[edgeToProcess];

        int startId = e.mStart;
        int endId = e.mEnd;
        int idToAdd = -1;

        if (!isPointAdded(startId)) 
        {
            idToAdd = startId;
        }
        else
        {
            idToAdd = endId;
        }

        outPoints.push_back(PointData(idToAdd, unorderedPoints[idToAdd].mX, unorderedPoints[idToAdd].mY));
        e.mAddedToMeshLoop = true;
        nextStartId = idToAdd;
    }
}

void PostProcessPoints(std::map<int, PointData>& allPoints, std::map<int, PointData>& outPoints)
{
    float maxNorm = 0.0f;

    // Step 1: Find the largest norm (distance from origin)
    for (const auto& [id, point] : allPoints) {
        float norm = std::sqrt(point.mX * point.mX + point.mY * point.mY);
        if (norm > maxNorm) {
            maxNorm = norm;
        }
    }

    // Step 2: Compute shrink factor to ensure all points are inside unit circle
    float shrinkFactor = (maxNorm >= 1.0f) ? 0.9999f / maxNorm : 1.0f;

    // Step 3: Scale and insert into outPoints
    for (const auto& [id, point] : allPoints) {
        float x = point.mX * shrinkFactor;
        float y = point.mY * shrinkFactor;
        outPoints.insert(std::make_pair(id, PointData(id, x, y)));
    }
}

void GetPointsFromBoundaryEdges(std::map<int, PointData>& allPoints, const std::set<EdgeData>& edges, std::map<int, PointData>& outPoints)
{
    for(const auto& e : edges)
    {
        int startId = e.mStart;
        int endId = e.mEnd;

        PointData startPoint = allPoints[startId];
        PointData endPoint = allPoints[endId];

        if (outPoints.find(startId) == outPoints.end()) 
        {
            outPoints.insert(std::make_pair(startId, PointData(startId, startPoint.mX, startPoint.mY)));
        } 

        if (outPoints.find(endId) == outPoints.end()) 
        {
            outPoints.insert(std::make_pair(endId, PointData(endId, endPoint.mX, endPoint.mY)));
        } 

    }
}

void FindBoundaryEdgesAndPoints(const std::string& filename, std::vector<PointData>& outPoints, std::set<EdgeData>& outEdges)
{
    std::vector<EdgeData> allEdges;
    std::map<int, PointData> allPoints;
    MeshFileReader::ExtractEdgesAndPointsFromMesh(filename, allPoints, allEdges);

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

    std::map<int, PointData> preProcessedPoints;
    GetPointsFromBoundaryEdges(allPoints, outEdges, preProcessedPoints);

    std::map<int, PointData> processedUnorderedPoints;
    PostProcessPoints(preProcessedPoints, processedUnorderedPoints);

    std::vector<EdgeData> edgeVector(outEdges.begin(), outEdges.end());
    ReorderPoints(processedUnorderedPoints, edgeVector, outPoints);

    std::cout << "Edges:" << std::endl;
    for(const auto& e : outEdges)
    {
        std::cout << e.mStart << ", " << e.mEnd << std::endl;
    }

    std::cout << "Preprocessed Points:" << std::endl;
    for(const auto& p : preProcessedPoints)
    {
        std::cout << p.second.mId << ", " << p.second.mX << ", " << p.second.mY << std::endl;
    }

    std::cout << "Postprocessed Points:" << std::endl;
    for(const auto& p : outPoints)
    {
        std::cout << p.mId << ", " << p.mX << ", " << p.mY << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::set<EdgeData> edges;
    std::vector<PointData> points;

    FindBoundaryEdgesAndPoints("./data/SkeletonizationTrainingData/t-shaped.msh", points, edges);

    std::vector<PointData> uniquePoints;
    RemoveDuplicatePoints(points, uniquePoints);

    std::vector<PointData> cleanedPoints;
    RemoveColinearPoints(uniquePoints, cleanedPoints);

    const ovd::VoronoiDiagram* vd = SimpleVoronoi2D::GenerateVoronoi2D(cleanedPoints);
    Training::GenerateTrainingDataFromVoronoi(vd);

    return 0;
}