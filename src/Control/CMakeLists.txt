cmake_minimum_required(VERSION 2.8)

PROJECT(Control)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/../../inc/
  ${CMAKE_CURRENT_SOURCE_DIR}/../RealTimeUltrasteer
) 

SET(ControlSrcs RobotControl.cpp UnscentedKalmanFilter.cpp MotionController.cpp SerialBuffer.cpp SerialCommHelper.cpp RollDevice.cpp InsertionDevice.cpp DeviceNotification.cpp NeedleSteeringRobot.cpp)

SET(ControlHeaders NeedleSteeringRobot.h)

SET(ControlOtherHeaders RobotControl.h UnscentedKalmanFilter.h MotionController.h MotionControllerException.h SerialBuffer.h SerialCommHelper.h RollDevice.h InsertionDevice.h DeviceNotification.h)

# for generate qt aditional files 
QT4_WRAP_CPP(MOCSrcs ${ControlHeaders} )

# Packaging source code in same files 

SOURCE_GROUP("Generated" FILES
  ${MOCSrcs}
)

ADD_LIBRARY(Control STATIC
	${ControlSrcs}
	${ControlHeaders}
	${MOCSrcs}
	${ControlOtherHeaders}
	)

