cmake_minimum_required(VERSION 2.8)

PROJECT(RobotControlGUI)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_ImageViewerUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
  ../RobotHardware
) 

SET(RobotControlGUIsrcs main.cpp robotcontrol.cpp)

SET(RobotControlGUIHeaders robotcontrol.h)

SET(RobotControlUI robotcontrol.ui)

SET(RobotControlResources robotcontrol.qrc)

# for generate qt aditional files 
QT4_WRAP_UI(UISrcs ${RobotControlUI})
QT4_WRAP_CPP(MOCSrcs ${RobotControlGUIHeaders} )
QT4_ADD_RESOURCES(ResourcesRCC ${RobotControlResources})

# Packaging source code in same files 
SOURCE_GROUP("Resources" FILES
  ${RobotControlUI}
  ${RobotControlResources}
)

SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${ResourcesRCC}
)

ADD_EXECUTABLE(RobotControlGUI WIN32
	${RobotControlGUIsrcs}
	${RobotControlGUIHeaders}
	${UISrcs}
	${MOCSrcs}
	${ResourcesRCC}
	)

SET_TARGET_PROPERTIES(RobotControlGUI PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:windows)
 #Must be lowercase windows

SET_TARGET_PROPERTIES(RobotControlGUI PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:windows)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(RobotControlGUI RobotHardware ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})

