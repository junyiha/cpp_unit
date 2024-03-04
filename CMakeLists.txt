#
cmake_minimum_required(VERSION 3.0)

#
project(cpp_unit)

#
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

#
aux_source_directory(${CMAKE_SOURCE_DIR}/src/ SRC_FILES)

add_executable(cpp_unit ${SRC_FILES})

target_include_directories(cpp_unit PUBLIC ${CMAKE_SOURCE_DIR}/include/)