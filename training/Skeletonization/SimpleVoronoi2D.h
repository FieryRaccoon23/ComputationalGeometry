#pragma once

#include <vector>

class EdgeData;
class PointData;

namespace SimpleVoronoi2D
{
    void GenerateVoronoi2D(const std::vector<PointData>& points);
}