cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND)


INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ../Segment ../RPFileReader)

SET(VolumeCreatorHeaders VolumeCreator.h CubeVisualizer.h)
SET(VolumeCreatorSources VolumeCreator.cpp CubeVisualizer.cpp)

SOURCE_GROUP("Headers" FILES
	${VolumeCreatorHeaders}
	)
	
ADD_LIBRARY(VolumeCreator STATIC
	${VolumeCreatorHeaders}
	${VolumeCreatorSources}
	)

