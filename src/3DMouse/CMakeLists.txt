cmake_minimum_required(VERSION 2.8)

PROJECT(3DMouse)

IF(COMMAND CMAKE_POLICY)
	   CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}
) 

SET(3DMouseSrcs Mouse3DInput.cpp MouseParameters.cpp)

SET(3DMouseHeaders Mouse3DInput.h)

SET(3DMouseOtherHeaders I3dMouseParams.h MouseParameters.h)

# for generate qt aditional files 
QT4_WRAP_CPP(MOCSrcs ${3DMouseHeaders} )

ADD_LIBRARY(3DMouse STATIC
	${3DMouseSrcs}
	${3DMouseHeaders}
	${MOCSrcs}
	${3DMouseOtherHeaders}
	)

