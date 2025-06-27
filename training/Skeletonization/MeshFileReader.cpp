#include "MeshFileReader.h"

#include <algorithm>
#include <cstdlib>
#include <set>
#include <sstream>

#include "EdgeData.h"
#include "PointData.h"

namespace MeshFileReader
{
    void TrimFromEnd(std::string &s) 
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](unsigned char ch) { return ch != '\r' && ch != '\n'; }).base(), s.end());
    }

    void ExtractEdgesAndPointsFromMesh(const std::string& filename, std::map<int, PointData>& outPoints, std::vector<EdgeData>& outEdges) 
    {
        std::ifstream infile(filename);
        std::string line;
        int num_nodes = 0, num_elements = 0;

        while (std::getline(infile, line)) 
        {
            TrimFromEnd(line);  // remove \r and \n from end

            if (line == "$Nodes") {
                std::getline(infile, line);
                num_nodes = std::stoi(line);
                for (int i = 0; i < num_nodes; ++i) {
                    std::getline(infile, line);
                    std::istringstream iss(line);
                    int id;
                    float x,y,z;
                    iss >> id >> x >> y >> z;
                    outPoints.insert(std::make_pair(id, PointData(id, x, y)));
                }
            }

            if (line == "$Elements") {
                std::getline(infile, line);
                num_elements = std::stoi(line);
                for (int i = 0; i < num_elements; ++i) {
                    std::getline(infile, line);
                    std::istringstream iss(line);
                    int id, type, tag, n1, n2, n3;
                    iss >> id >> type >> tag >> n1 >> n2 >> n3;

                    if (type == 2) {  // Type 2 = triangle
                        // Add all 3 edges
                        outEdges.emplace_back(EdgeData(std::min(n1,n2), std::max(n1,n2)));
                        outEdges.emplace_back(EdgeData(std::min(n2,n3), std::max(n2,n3)));
                        outEdges.emplace_back(EdgeData(std::min(n3,n1), std::max(n3,n1)));
                    }
                }
            }
        }

    }

    void ExtractPointsFromMesh(const std::string& filename, std::set<PointData>& outPoints)
    {

    }
}