cmake_minimum_required(VERSION 2.8)

SET(OPENCV_DIR "D:/Build/OpenCV2.2")
FIND_PACKAGE(OpenCV)
Project(RealTimeUltrasteer)


FIND_PACKAGE(OPENCV)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../external/alglib ${CMAKE_CURRENT_BINARY_DIR}/alglib)
ADD_SUBDIRECTORY(NeedleEstimator)
ADD_SUBDIRECTORY(Control)
ADD_SUBDIRECTORY(RPFileReader)
ADD_SUBDIRECTORY(UnitTests)
