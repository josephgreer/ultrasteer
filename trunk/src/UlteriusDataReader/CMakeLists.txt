cmake_minimum_required(VERSION 2.8)

PROJECT(UlteriusDataReader)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Segment
  ${CMAKE_CURRENT_SOURCE_DIR}/../RPFileReader
  ${CMAKE_CURRENT_SOURCE_DIR}/../RPFileWriter
  ${CMAKE_CURRENT_SOURCE_DIR}/../UlteriusDataGrabber
  ${UlteriusDir}/inc
) 

ADD_EXECUTABLE(UlteriusDataReader WIN32
	UlteriusDataReader.cpp
)

SET_TARGET_PROPERTIES(UlteriusDataReader PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:windows)
 #Must be lowercase windows

SET_TARGET_PROPERTIES(UlteriusDataReader PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:windows)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(UlteriusDataReader RPFileReader RPFileWriter ${OpenCV_LIBS} ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${UlteriusDir}/lib/ulterius_old.lib)

