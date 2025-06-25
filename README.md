# Computational Geometry and ML
Using CGAL to generate data and training to create Delaunay triangulation.

## Setup
1) Create a libs folder
2) Add CGAL, boost and Eigen libraries (change the CMakeList.txt file depending on the versions of each)
3) Create a Pytorch folder
4) Download libtorch for C++ (change the CMakeList.txt file depending on the version of libtorch)
5) All data goes into data folder. Folder data/csv contains the csv files for points and edges. Folder data/mesh contains the mesh
6) Install pytorch
7) Get PointTransformerLayer model: https://github.com/lucidrains/point-transformer-pytorch

## How to
1) Run ./run_cmake "project name" to create make files (first parameter folder name like DelaunayTriangulation, second parameter is training/inference, third parameter release/debug)
2) Run ./make.sh to build
3) Run the application in build folder by running ./build_training/app
4) Cleanup build folder using the command ./clean_build.sh and ./clean_data.sh to clean data folder

## License

This project is licensed under the CC BY-NC-ND 4.0 License.  
You may not use it for commercial purposes or create derivative works.  
See the [LICENSE](./LICENSE.md) file for more details.