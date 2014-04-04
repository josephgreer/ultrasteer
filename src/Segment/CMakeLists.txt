cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src)
ADD_LIBRARY(Segment STATIC
	SegmentCore.h
	Segment.h
	Segment.cpp
	)

