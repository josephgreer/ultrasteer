cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND)

# Find QT 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )

FIND_PACKAGE(Qt4 COMPONENTS QtOpenGL REQUIRED)
#INCLUDE( ${QT_QtOpenGL_USE_FILE} )

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/TransferFunctionWidget
  ${CMAKE_CURRENT_SOURCE_DIR}/TransferFunctionWidget/VTKInterface
  ${CMAKE_CURRENT_SOURCE_DIR}/Core
  ${QT_QTOPENGL_INCLUDE_DIR}
 )

file(GLOB CTKTransferFunctionSrc
    "*.cxx"
    "*.cpp"
    "TransferFunctionWidget/*.cxx"
    "TransferFunctionWidget/*.cpp"
    "Core/*.cxx"
    "Core/*.cpp"
)

file(GLOB CTKTransferFunctionHeader
    "*.h"
    "TransferFunctionWidget/*.h"
    "Core/*.h"
)


file(GLOB CTKVTKSrc
    "TransferFunctionWidget/VTKInterface/*.cxx"
    "TransferFunctionWidget/VTKInterface/*.cpp"
)
file(GLOB CTKVTKHeader
    "TransferFunctionWidget/VTKInterface/*.h"
)

QT4_WRAP_CPP(MOCCTKTrans ${CTKTransferFunctionHeader} ${CTKVTKHeader})

SOURCE_GROUP("Headers" FILES
	${CTKTransferFunctionHeader}
	)
	
SOURCE_GROUP("CTKVTKHeader" FILES
	${CTKVTKHeader}
)

SOURCE_GROUP("CTKVTK" FILES
	${CTKVTKSrc}
)

SOURCE_GROUP("Generated" FILES
	${MOCCTKTrans}
	#${MOCCTKVTK}
)
	
ADD_LIBRARY(CTKTransferFunctionEditor STATIC
	${CTKTransferFunctionSrc}
	${CTKTransferFunctionHeader}
	${CTKVTKSrc}
	${CTKVTKHeader}
	${MOCCTKTrans}
	#${MOCCTKVTK}
	)

