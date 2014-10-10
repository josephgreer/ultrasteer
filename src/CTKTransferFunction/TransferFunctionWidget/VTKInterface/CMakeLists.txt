cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR} 
 )
 
file(GLOB CTKTransferFuncWidgetVTKInterface
    "*.cxx"
    "*.cpp"
)

file(GLOB CTKTransferFunctionWidgetVTKInterfaceHeader
    "*.h"
)

