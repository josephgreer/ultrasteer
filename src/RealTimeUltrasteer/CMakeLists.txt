cmake_minimum_required(VERSION 2.8)

PROJECT(RealTimeUltrasteer)

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
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_ImageViewerUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../inc/
  ${CMAKE_CURRENT_SOURCE_DIR}/../ControlAlgorithms/  
  ${CMAKE_CURRENT_SOURCE_DIR}/../Segment/
  ${CMAKE_CURRENT_SOURCE_DIR}/../3DMouse/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src
) 

SET(ultrasteerSrcs main.cpp Propello.cpp PropelloDisplay.cpp PropelloBDisplay.cpp SaveDlg.cpp StdAfx.cpp)

SET(ultrasteerHeaders Propello.h SaveDlg.h PropelloBDisplay.h)

SET(ultrasteerOtherHeaders stdafx.h SaveDlg.h PropelloDisplay.h Propello.h)

SET(ultrasteerUI save.ui propello.ui)

SET(ultrasteerResources propello.qrc)

# for generate qt aditional files 
QT4_WRAP_UI(UISrcs ${ultrasteerUI})
QT4_WRAP_CPP(MOCSrcs ${ultrasteerHeaders} )
QT4_ADD_RESOURCES(ResourcesRCC ${ultrasteerResources})

# Packaging source code in same files 
SOURCE_GROUP("Resources" FILES
  ${ultrasteerUI}
  ${ultrasteerResources}
)

SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
)

ADD_EXECUTABLE(RealTimeUltrasteer WIN32
	${ultrasteerSrcs}
	${ultrasteerHeaders}
	${UISrcs}
	${MOCSrcs}
	${ultrasteerOtherHeaders}
	${ResourcesRCC}
	)

SET_TARGET_PROPERTIES(RealTimeUltrasteer PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:windows)
 #Must be lowercase windows

SET_TARGET_PROPERTIES(RealTimeUltrasteer PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:windows)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(RealTimeUltrasteer ControlAlgorithms alglib Segment 3DMouse ${OpenCV_LIBS} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/porta.lib ${ITK_LIBRARIES} ${VTK_LIBRARIES})

