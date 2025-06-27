#include "SimpleVoronoi2D.h"

#include <string>
#include <iostream>
#include <cmath>

#include "voronoidiagram.hpp"
#include "version.hpp"
#include "common/point.hpp"
#include "utility/vd2svg.hpp"

#include "EdgeData.h"
#include "PointData.h"

#define TOLERANCE1 1
#define TOLERANCE2 10

namespace SimpleVoronoi2D
{
    void GenerateVoronoi2D(const std::map<int, PointData>& points)
    {
        ovd::VoronoiDiagram* vd = new ovd::VoronoiDiagram(TOLERANCE1, TOLERANCE2);

        std::vector<int> vertexIds;
        for (const auto& [pointId, pointCoordinate] : points) 
        {
            ovd::Point p = ovd::Point(pointCoordinate.mX, pointCoordinate.mY);
            vertexIds.push_back( vd->insert_point_site(p) );   
        }

        for (unsigned int n = 0; n < vertexIds.size(); ++n)
         {
            int next = n + 1;
            if (n == (vertexIds.size() - 1))
            {
                next=0;
            }
            vd->insert_line_site( vertexIds[n], vertexIds[next]);
        }

        std::cout << vd->print();
        vd2svg("SimpleVoronoi2D.svg", vd);
        delete vd;
    }
}