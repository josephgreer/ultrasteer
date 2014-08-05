cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
ADD_LIBRARY(VolumeCreator STATIC
	VolumeCreator.cpp
	)

