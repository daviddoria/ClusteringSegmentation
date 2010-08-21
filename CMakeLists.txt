cmake_minimum_required(VERSION 2.6)

set(VTKAdditions /media/portable/VTK_Additions)
INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${VTKAdditions})

PROJECT(ClusteringSegmentation)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(ClusteringSegmentation ClusteringSegmentation.cpp)
TARGET_LINK_LIBRARIES(ClusteringSegmentation vtkHybrid vtkInfovis)

