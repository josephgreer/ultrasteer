cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR} 
 )
 
file(GLOB CTKTransferFuncWidget
    "*.cxx"
    "*.cpp"
)

file(GLOB CTKTransferFunctionWidgetHeader
    "*.h"
)

ADD_SUBDIRECTOY(VTKInterface)

