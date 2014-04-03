cmake_minimum_required(VERSION 2.8)

SET(OPENCV_DIR "D:/Build/OpenCV2.2")
FIND_PACKAGE(OpenCV)
Project(RealTimeUltrasteer)
ADD_SUBDIRECTORY(NeedleEstimator)
ADD_SUBDIRECTORY(Control)
