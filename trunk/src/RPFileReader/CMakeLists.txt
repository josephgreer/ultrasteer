cmake_minimum_required(VERSION 2.8)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(
	${OPENCV_INCLUDE_DIRS} 
	${CMAKE_CURRENT_SOURCE_DIR}/../Segment
	${CMAKE_CURRENT_SOURCE_DIR}/../UlteriusDataGrabber
	${UlteriusDir}/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../../external/
	${CMAKE_CURRENT_SOURCE_DIR}/../../external/armadillo/include
)

ADD_LIBRARY(RPFileReader STATIC
    RPFileReader.cpp
    RPFileReader.h    
    RPUlterius.h
    RPUlterius.cpp
    RPProcess.cpp
    RPProcess.h
)

