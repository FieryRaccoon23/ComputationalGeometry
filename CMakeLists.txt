# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.23)

project( ComputationalGeometry LANGUAGES CXX CUDA)

# Enable debug symbols and disable optimization
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
# set(CMAKE_CUDA_FLAGS_DEBUG "-G -O0")

#set directories
set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/libs/boost_1_88_0")
set(CGAL_DIR "${PROJECT_SOURCE_DIR}/libs/CGAL-6.0.1")
set(EIGEN3_INCLUDE_DIR "libs/eigen-3.4.0")
set(TORCH_DIR "${PROJECT_SOURCE_DIR}/Pytorch/cuda12_8/libtorch")

# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()

# include Eigen
include_directories(${EIGEN3_INCLUDE_DIR})

# include libtorch
set(CMAKE_PREFIX_PATH "${TORCH_DIR}")
find_package(Torch REQUIRED)

# Creating entries for all C++ files with "main" routine
# ##########################################################

# Specify default if not provided
if(NOT DEFINED SOURCE_FOLDER)
  set(SOURCE_FOLDER "src")
endif()

# Include subdirectories for headers
include_directories(${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER})

# Automatically collect all .cpp, .h and .cu files in ${SOURCE_FOLDER} and subfolders
file(GLOB_RECURSE SOURCE_FILES
     "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/*.cpp"
     "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/*.cxx")

file(GLOB_RECURSE CUDA_FILES
     "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/*.cu")

file(GLOB_RECURSE HEADER_FILES
     "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/*.h"
     "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/*.hpp")

# Create executable
add_executable(app ${SOURCE_FILES} ${CUDA_FILES} ${HEADER_FILES})

# Properly link CGAL (and implicitly include its headers)
target_link_libraries(app CGAL::CGAL)

# Also link Boost headers if needed (optional)
target_include_directories(app PRIVATE ${Boost_INCLUDE_DIRS})

# Link Eigen
target_include_directories(app PRIVATE ${EIGEN3_INCLUDE_DIR})

# Link libtorch
target_include_directories(app PRIVATE ${TORCH_INCLUDE_DIRS})
target_link_libraries(app "${TORCH_LIBRARIES}")
set_property(TARGET app PROPERTY CXX_STANDARD 17)

set_target_properties(app PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_STANDARD 14
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/${OUTPUT_FOLDER}"
)

