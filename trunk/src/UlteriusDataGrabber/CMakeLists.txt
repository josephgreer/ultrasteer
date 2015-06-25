cmake_minimum_required(VERSION 2.8)

PROJECT(UlteriusDataGrabber)

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
   INCLUDE(${VTK_USE_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ../Segment
  ../RPFileReader
  ../RPFileWriter
  ${UlteriusDir}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/armadillo/include
) 

ADD_EXECUTABLE(UlteriusDataGrabber
	UlteriusDataGrabber.cpp
	UlteriusInterface.cpp
	UlteriusInterface.h
)

#SET_TARGET_PROPERTIES(UlteriusDataGrabber PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:windows)
 #Must be lowercase windows

#SET_TARGET_PROPERTIES(UlteriusDataGrabber PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:windows)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(UlteriusDataGrabber RPFileReader Segment ${OpenCV_LIBS} ${ITK_LIBRARIES} ${VTK_LIBRARIES} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${UlteriusDir}/lib/ulterius_old.lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/lapack/liblapack.lib ${CMAKE_CURRENT_SOURCE_DIR}/../../external/lapack/libblas.lib)

