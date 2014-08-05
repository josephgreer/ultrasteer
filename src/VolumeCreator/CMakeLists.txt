cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

SET(VolumeCreatorHeaders VolumeCreator.h)
SET(VolumeCreatorSources VolumeCreator.cpp)

SOURCE_GROUP("Headers" FILES
	${VolumeCreatorHeaders}
	)
	
ADD_LIBRARY(VolumeCreator STATIC
	${VolumeCreatorHeaders}
	${VolumeCreatorSources}
	)

