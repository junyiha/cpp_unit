#
cmake_minimum_required(VERSION 3.20)

#
project(cpp_unit)

#
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

#
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "find openssl library, include directory: " ${OpenSSL_INCLUDE_DIR})
else(OpenSSL_FOUND)
    message(FATAL_ERROR "Can not find openssl library")
endif(OpenSSL_FOUND)

#
find_package(Boost 1.83.0 REQUIRED COMPONENTS system filesystem thread)
if(Boost_FOUND)
    message(STATUS "find boost library, include directory: " ${Boost_INCLUDE_DIR})
    message(STATUS "find boost library, libraries directory: " ${Boost_LIBRARY_DIRS})
    message(STATUS "find boost library, libraries libraries: " ${Boost_LIBRARIES})
else(Boost_FOUND)
    message(FATAL_ERROR "Can not find Boost!!!")
endif(Boost_FOUND)

# 
find_package(GLOG REQUIRED PATHS ${CMAKE_SOURCE_DIR}/third_party/glog/lib/cmake/glog/ NO_DEFAULT_PATH)
if(GLOG_FOUND)
    message(STATUS "find GLOG library, include directory: " ${GLOG_INCLUDE_DIR})
else(GLOG_FOUND)
    message(FATAL_ERROR "Can not find GLOG!!!")
endif(GLOG_FOUND)

#
find_package(GFLAGS REQUIRED PATHS ${CMAKE_SOURCE_DIR}/third_party/gflags/lib/cmake/gflags/ NO_DEFAULT_PATH)
if(GFLAGS_FOUND)
    message(STATUS "find GFLAGS library, include directory: " ${GFLAGS_INCLUDE_DIR})
    message(STATUS "GFLAGS libraries: " ${GFLAGS_LIBRARIES})
else(GFLAGS_FOUND)
    message(FATAL_ERROR "Can not find GFLAGS!!!")
endif(GFLAGS_FOUND)

#
find_package(yaml-cpp REQUIRED PATHS ${CMAKE_SOURCE_DIR}/third_party/yaml-cpp/lib/cmake/yaml-cpp/ NO_DEFAULT_PATH)
if(yaml-cpp_FOUND)
    message(STATUS "find yaml-cpp library, include directory: " ${YAML_CPP_INCLUDE_DIR})
else(yaml-cpp_FOUND)
    message(FATAL_ERROR "Can not find yaml-cpp library")
endif(yaml-cpp_FOUND)

#
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/data/usr/local/ffmpeg/lib/pkgconfig")
pkg_check_modules(ffmpeg REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil)

#
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "find opencv library, include directory: " ${OpenCV_INCLUDE_DIRS})
    message(STATUS "find opencv library, link directory: " ${OpenCV_LIBS})
else(OpenCV_FOUND)
    message(FATAL_ERROR "Can not find opencv library")
endif(OpenCV_FOUND)

#
aux_source_directory(${CMAKE_SOURCE_DIR}/src/ SRC_FILES)

#
aux_source_directory(${CMAKE_SOURCE_DIR}/third_party/mongoose/ MONGOOSE_SRC_FILES)

add_executable(cpp_unit ${SRC_FILES} ${MONGOOSE_SRC_FILES})

target_include_directories(cpp_unit PUBLIC ${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/third_party/ ${OpenCV_INCLUDE_DIRS})

target_link_libraries(cpp_unit glog::glog gflags yaml-cpp::yaml-cpp Boost::filesystem OpenSSL::SSL OpenSSL::Crypto PkgConfig::ffmpeg ${OpenCV_LIBS})