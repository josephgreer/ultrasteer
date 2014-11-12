cmake_minimum_required(VERSION 2.8)

PROJECT(RobotHardware)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_ImageViewerUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
) 

SET(ControlSrcs MotionController.cpp SerialBuffer.cpp SerialCommHelper.cpp RollDevice.cpp InsertionDevice.cpp DeviceNotification.cpp NeedleSteeringRobot.cpp)

SET(ControlHeaders NeedleSteeringRobot.h)

SET(ControlOtherHeaders MotionController.h MotionControllerException.h SerialBuffer.h SerialCommHelper.h RollDevice.h InsertionDevice.h DeviceNotification.h)

# for generate qt aditional files 
QT4_WRAP_CPP(MOCSrcs ${ControlHeaders} )

# Packaging source code in same files 

SOURCE_GROUP("Generated" FILES
  ${MOCSrcs}
)

ADD_LIBRARY(RobotHardware STATIC
	${ControlSrcs}
	${ControlHeaders}
	${MOCSrcs}
	${ControlOtherHeaders}
	)

