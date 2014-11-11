cmake_minimum_required(VERSION 2.8)

PROJECT(UlteriusDataRecorder)

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
  C:/Ulterius/sdk611/ulterius/inc
) 

ADD_EXECUTABLE(UlteriusDataRecorder WIN32
	UlteriusDataRecorder.cpp
)

SET_TARGET_PROPERTIES(UlteriusDataRecorder PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:windows)
 #Must be lowercase windows

SET_TARGET_PROPERTIES(UlteriusDataRecorder PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:windows)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(UlteriusDataRecorder RPFileReader RPFileWriter ${OpenCV_LIBS} ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} C:/Ulterius/sdk611/ulterius/lib/ulterius_old.lib)

