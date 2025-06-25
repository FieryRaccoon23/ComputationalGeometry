#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/point_generators_2.h>

#include <fstream>
#include <vector>
#include <cstdlib>
#include <set>

#include <chrono>

#include <torch/torch.h>
#include <torch/script.h>

#define MESH_DIR "./data/mesh/"
#define CSV_DIR "./data/csv/"
#define NUM_OF_POINTS 4

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;

class PointData2D
{
public:
    PointData2D(){}
    PointData2D(Point_2 point, int id)
    {
        mPoint = point;
        mID = id;
    }

    Point_2 mPoint;
    int     mID = -1;
};

void SaveDataToFiles(const std::map<Point_2, int>& point_ids, const std::set<std::pair<Point_2, Point_2>>& edges, const std::vector<std::array<PointData2D, 3>>& triangles, int index)
{
    std::string points_filename = std::string(CSV_DIR) + "points_" + std::to_string(index) + ".csv";
    std::ofstream points_file(points_filename);
    for (const auto& p : point_ids) 
    {
        //points_file << p.second << "," << p.first.x() << "," << p.first.y() << "\n";
        points_file << p.first.x() << "," << p.first.y() << "\n";
    }
    points_file.close();

    std::string edges_filename = std::string(CSV_DIR) + "edges_" + std::to_string(index) + ".csv";
    std::ofstream edges_file(edges_filename);
    for (const auto& e : edges) 
    {
        //edges_file << e.first << "," << e.second << "\n";
        edges_file << e.first.x() << "," << e.first.y() << ","  << e.second.x() << "," << e.second.y() << "\n";
    }
    edges_file.close();

    // std::string triangles_filename = std::string(CSV_DIR) + "triangles_" + std::to_string(index) + ".csv";
    // std::ofstream triangles_file(triangles_filename);
    // for (const auto& t : triangles) 
    // {
    //     triangles_file << t[0] << "," << t[1]  << "," << t[2] << "\n";
    // }
    // triangles_file.close();
}

void GenerateDelaunayTriangulation(int index) 
{
    std::string filename = std::string(MESH_DIR) + "delaunay2D_" + std::to_string(index) + ".msh";
    std::ofstream out(filename);            
    if (!out) {
        std::cerr << "Cannot open output file.\n";
        return;
    }

    const int n = NUM_OF_POINTS;  // Number of points
    std::vector<Point_2> points;
    CGAL::Random_points_in_square_2<Point_2> gen(1.0);

    for (int i = 0; i < n; ++i) {
        points.push_back(*gen++);
    }

     // ⏱ Start timer
    auto start = std::chrono::high_resolution_clock::now();
    Delaunay dt;
    dt.insert(points.begin(), points.end());

    // ⏱ End timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Delaunay triangulation took " << elapsed.count() << " seconds.\n";

    // Start writing the .msh file in Gmsh ASCII format (version 2.2)
    out << "$MeshFormat\n2.2 0 8\n$EndMeshFormat\n";

    // Write nodes
    out << "$Nodes\n" << dt.number_of_vertices() << "\n";
    std::map<Point_2, int> point_ids;
    int id = 1;
    for (auto vit = dt.finite_vertices_begin(); vit != dt.finite_vertices_end(); ++vit) {
        //point_ids.insert({id, vit->point()});
        point_ids[vit->point()] = id;
        out << id << " " << vit->point().x() << " " << vit->point().y() << " 0\n";
        ++id;
    }
    out << "$EndNodes\n";

    // Write elements (triangles)
    std::vector<std::array<PointData2D, 3>> triangles;
    for (auto fit = dt.finite_faces_begin(); fit != dt.finite_faces_end(); ++fit) {
        std::array<PointData2D, 3> tri = {
            PointData2D(fit->vertex(0)->point(), point_ids[fit->vertex(0)->point()]),
            PointData2D(fit->vertex(1)->point(), point_ids[fit->vertex(1)->point()]),
            PointData2D(fit->vertex(2)->point(), point_ids[fit->vertex(2)->point()])
        };
        triangles.push_back(tri);
    }

    std::set<std::pair<Point_2, Point_2>> edges;

    auto make_edge = [&](PointData2D first, PointData2D second) 
    {
        // bool isFirstLow = false;
        // if(first.x() < second.x())
        // {
        //     isFirstLow = true;
        // }
        // else if(first.x() == second.x())
        // {
        //     isFirstLow = first.y() < second.y();
        // }

        //return isFirstLow ? std::make_pair(first, second) : std::make_pair(second, first);

        bool isFirstLow = first.mID < second.mID;
        return isFirstLow ? std::make_pair(first.mPoint, second.mPoint) : std::make_pair(second.mPoint, first.mPoint);

    };

    out << "$Elements\n" << triangles.size() << "\n";
    for (std::size_t i = 0; i < triangles.size(); ++i) {
        out << i+1 << " 2 0 " << triangles[i][0].mID << " "
            << triangles[i][1].mID << " " << triangles[i][2].mID << "\n";

        // save edges but make sure that first edge is always min so we can avoid duplicate edges
        edges.insert(make_edge(triangles[i][0], triangles[i][1]));
        edges.insert(make_edge(triangles[i][1], triangles[i][2]));
        edges.insert(make_edge(triangles[i][2], triangles[i][0]));
    }
    out << "$EndElements\n";

    out.close();
    std::cout << "Delaunay triangulation written to delaunay2D.msh\n";

    // torch::Tensor t = torch::rand({2, 3});
    // std::cout << t << std::endl;

    SaveDataToFiles(point_ids, edges, triangles, index);
}

int main(int argc, char* argv[])
{
    int numberOfDataFiles = 1;
    if (argc >= 2) 
    {
        numberOfDataFiles = std::atoi(argv[1]);
    }

    std::cout << "Starting generation of triangles...\n";
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < numberOfDataFiles; ++i)
    {   
        GenerateDelaunayTriangulation(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Generating " << numberOfDataFiles << " number of data files took: "  << elapsed.count() << " seconds.\n";

    return 0;
}