#pragma once

#include <vector>
#include <map>

class EdgeData;
class PointData;

namespace SimpleVoronoi2D
{
    void GenerateVoronoi2D(const std::map<int, PointData>& points);
}