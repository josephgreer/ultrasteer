cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(OPENCV)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS})
ADD_LIBRARY(NeedleEstimator STATIC
	NeedleEstimatorCore.h
	NeedleFitter.h
	NeedleFitter.cpp
	)

