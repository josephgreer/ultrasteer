cmake_minimum_required(VERSION 2.8)

SET(VTK_DIR "D:/Build/VTK")
SET(ITK_DIR "D:/Build/ITK")

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ELSE(VTK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(VTK_FOUND)


INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src)
ADD_LIBRARY(Segment STATIC
	SegmentCore.h
	Segment.h
	Segment.cpp
	)

