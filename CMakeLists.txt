cmake_minimum_required(VERSION 2.6)

PROJECT(ClusteringSegmentation)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(ClusteringSegmentation ClusteringSegmentation.cpp vtkClusteringSegmentation.cxx)
TARGET_LINK_LIBRARIES(ClusteringSegmentation vtkHybrid vtkInfovis)

