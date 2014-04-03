cmake_minimum_required(VERSION 2.8)

file(GLOB alglib_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

ADD_LIBRARY(alglib STATIC
	${alglib_SRC}
	)

