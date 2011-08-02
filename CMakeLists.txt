cmake_minimum_required(VERSION 2.6)

PROJECT(ClusteringSegmentation)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(ClusteringSegmentationDemo ClusteringSegmentationDemo.cpp vtkClusteringSegmentation.cxx)
TARGET_LINK_LIBRARIES(ClusteringSegmentationDemo ${VTK_LIBRARIES})

ADD_EXECUTABLE(ClusteringSegmentationExample ClusteringSegmentationExample.cpp vtkClusteringSegmentation.cxx)
TARGET_LINK_LIBRARIES(ClusteringSegmentationExample ${VTK_LIBRARIES})
