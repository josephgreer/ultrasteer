cmake_minimum_required(VERSION 2.8)

PROJECT(RPFileWriter)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ../Segment
  ../RPFileReader
  ${UlteriusDir}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/armadillo/include
) 

SET(RPFileWriterSrcs RPFileWriter.cpp)

SET(RPFileWriterHeaders RPFileWriter.h)

ADD_LIBRARY(RPFileWriter STATIC
	${RPFileWriterSrcs}
	${RPFileWriterHeaders}
	)