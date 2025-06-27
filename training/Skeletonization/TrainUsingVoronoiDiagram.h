#pragma once

#include <vector>

namespace ovd 
{
    class VoronoiDiagram;
}

namespace Training
{
    void GenerateTrainingDataFromVoronoi(const ovd::VoronoiDiagram* vd);
}