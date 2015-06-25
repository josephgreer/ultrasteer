cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(VTK_FOUND)


INCLUDE_DIRECTORIES(
	${OPENCV_INCLUDE_DIRS} 
	${CMAKE_CURRENT_SOURCE_DIR}/../RTUltrasteer
	${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src
	${CMAKE_CURRENT_SOURCE_DIR}/../../external/armadillo/include
)

ADD_LIBRARY(Segment STATIC
	Segment.h
	Segment.cpp
	ParticleFilter.h
	ParticleFilter.cpp
	RTCore.h
	RTMath.h
	RTMath.cpp
	RTMatrixMath.h
	RTUtil.h
	RTUtil.cpp
	)

