cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(OPENCV)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib ${CMAKE_CURRENT_BINARY_DIR}/../alglib)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src)
ADD_LIBRARY(NeedleEstimator STATIC
	NeedleEstimatorCore.h
	NeedleFitter.h
	NeedleFitter.cpp
	)

