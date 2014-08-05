cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ELSE(VTK_FOUND)
	MESSAGE(FATAL_ERROR "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(VTK_FOUND)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../Segment ${CMAKE_CURRENT_SOURCE_DIR}/../Control ${CMAKE_CURRENT_SOURCE_DIR}/../RPFileReader ${CMAKE_CURRENT_SOURCE_DIR}/../../external/alglib/src)

FILE(GLOB cppUnitFiles
    "${CMAKE_CURRENT_SOURCE_DIR}/CppUnitLite/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/CppUnitLite/*.h"
)

FILE(GLOB unitTestFiles
	"*.cpp"
	"*.h"
)

SOURCE_GROUP(CppUnitLite FILES ${cppUnitFiles})

ADD_EXECUTABLE(UnitTests WIN32
	${cppUnitFiles}
	${unitTestFiles}
)

SET_TARGET_PROPERTIES(UnitTests PROPERTIES LINK_FLAGS_DEBUG /SUBSYSTEM:console)
 #Must be lowercase windows

SET_TARGET_PROPERTIES(UnitTests PROPERTIES LINK_FLAGS_RELEASE /SUBSYSTEM:console)
 #Must be lowercase windows

TARGET_LINK_LIBRARIES(UnitTests alglib Segment RPFileReader ${OpenCV_LIBS} ${ITK_LIBRARIES} ${VTK_LIBRARIES})
#${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} 
