cmake_minimum_required(VERSION 2.8)

PROJECT(ControlAlgorithms)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src  
  ${CMAKE_CURRENT_SOURCE_DIR}/../Segment/
  ${CMAKE_CURRENT_SOURCE_DIR}/../VolumeCreator
  ${CMAKE_CURRENT_SOURCE_DIR}/../RobotHardware/
  ${CMAKE_CURRENT_SOURCE_DIR}/../RPFileReader/
  ${CMAKE_CURRENT_SOURCE_DIR}/../RTUltrasteer/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/armadillo/include
) 

ADD_LIBRARY(ControlAlgorithms STATIC
	ControlAlgorithms.h
	UnscentedKalmanFilter.h
	SegmentationAlgorithms.h
	ControlAlgorithms.cpp
	UnscentedKalmanFilter.cpp
	SegmentationAlgorithms.cpp	
	)

