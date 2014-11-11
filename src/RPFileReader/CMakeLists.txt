cmake_minimum_required(VERSION 2.8)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(
	${OPENCV_INCLUDE_DIRS} 
	${CMAKE_CURRENT_SOURCE_DIR}/../Segment
	${CMAKE_CURRENT_SOURCE_DIR}/../UlteriusDataGrabber
	C:/Ulterius/sdk611/ulterius/inc
	${CMAKE_CURRENT_SOURCE_DIR}/../../external/
)

ADD_LIBRARY(RPFileReader STATIC
    RPFileReader.cpp
    RPFileReader.h    
    RPUlterius.h
    RPUlterius.cpp
    RPProcess.cpp
    RPProcess.h
)

