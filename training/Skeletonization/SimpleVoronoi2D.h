#pragma once

#include <vector>

class EdgeData;
class PointData;

namespace ovd 
{
    class VoronoiDiagram;
}

namespace SimpleVoronoi2D
{
    const ovd::VoronoiDiagram* GenerateVoronoi2D(const std::vector<PointData>& points);
}