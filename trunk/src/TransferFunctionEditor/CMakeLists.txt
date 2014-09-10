cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ../Segment ../RPFileReader ../RTUltrasteer)

file(GLOB TransferFuncSrc
    "*.cxx"
    "*.cpp"
)
file(GLOB TransferFuncHeader
    "*.h"
)

SOURCE_GROUP("Headers" FILES
	${TransferFuncHeader}
	)
	
ADD_LIBRARY(TransferFunctionEditor STATIC
	${TransferFuncHeader}
	${TransferFuncSrc}
	)

