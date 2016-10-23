cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(
   ${OpenCV_INCLUDE_DIRS}
   ${CMAKE_CURRENT_SOURCE_DIR} 
   ../Segment 
   ../RPFileReader 
   ../RTUltrasteer 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/armadillo/include
	)

SET(VolumeCreatorHeaders VolumeCreator.h CubeVisualizer.h)
SET(VolumeCreatorSources VolumeCreator.cpp CubeVisualizer.cpp)

SOURCE_GROUP("Headers" FILES
	${VolumeCreatorHeaders}
	)
	
ADD_LIBRARY(VolumeCreator STATIC
	${VolumeCreatorHeaders}
	${VolumeCreatorSources}
	)

