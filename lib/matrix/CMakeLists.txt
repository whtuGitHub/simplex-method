cmake_minimum_required(VERSION 2.8)

project(Matrix)
set(CMAKE_CXX_FLAGS "-std=c++11 -O3")

include_directories(
	${VECTOR_LIB}
)

file(GLOB MATRIX_SRCS
	*.cpp
)

add_library(matrix STATIC ${MATRIX_SRCS})
add_dependencies(matrix vector)

# add_library(matrix_so SHARED ${MATRIX_SRCS})
# add_dependencies(matrix_so vector_so)
