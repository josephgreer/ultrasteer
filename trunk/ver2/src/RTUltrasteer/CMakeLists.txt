cmake_minimum_required(VERSION 2.8)

PROJECT(RTUltrasteer)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)


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
ENDIF(VTK_FOUND)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_ImageViewerUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
) 

SET(ultrasteerSrcs main.cpp RTUltrasteer.cpp)

SET(ultrasteerHeaders RTUltrasteer.h)

SET(ultrasteerUI RTUltrasteer.ui)

SET(ultrasteerResources RTUltrasteer.qrc)

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

ADD_EXECUTABLE(RTUltrasteer WIN32
	${ultrasteerSrcs}
	${ultrasteerHeaders}
	${UISrcs}
	${MOCSrcs}
	${ResourcesRCC}
	)

SET_TARGET_PROPERTIES(RTUltrasteer PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:windows)
 #Must be lowercase windows

SET_TARGET_PROPERTIES(RTUltrasteer PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:windows)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(RTUltrasteer ${OpenCV_LIBS} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${ITK_LIBRARIES} ${VTK_LIBRARIES})

