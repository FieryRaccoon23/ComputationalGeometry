#pragma once

#include <fstream>
#include <vector>

class EdgeData;

namespace MeshFileReader
{
    void TrimFromEnd(std::string &s);
    void ExtractEdgesFromMesh(const std::string& filename, std::vector<EdgeData>& outEdges);
}