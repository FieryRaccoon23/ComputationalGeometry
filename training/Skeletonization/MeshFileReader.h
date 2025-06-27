#pragma once

#include <fstream>
#include <vector>
#include <map>

class EdgeData;
class PointData;

namespace MeshFileReader
{
    void TrimFromEnd(std::string &s);
    void ExtractEdgesAndPointsFromMesh(const std::string& filename, std::map<int, PointData>& outPoints, std::vector<EdgeData>& outEdges);
}