cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR} 
 )
 
file(GLOB CTKTransferFuncCore
    "*.cxx"
    "*.cpp"
)

file(GLOB CTKTransferFunctionCoreHeader
    "*.h"
)

