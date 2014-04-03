cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../NeedleEstimator)
ADD_LIBRARY(RPFileReader STATIC
    RPFileReader.cpp
    RPFileReader.h    
)

