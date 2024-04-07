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
pkg_check_modules(ffmpeg REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil libswscale)

#
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "find opencv library, include directory: " ${OpenCV_INCLUDE_DIRS})
    message(STATUS "find opencv library, link directory: " ${OpenCV_LIBS})
else(OpenCV_FOUND)
    message(FATAL_ERROR "Can not find opencv library")
endif(OpenCV_FOUND)

#
find_package(CURL REQUIRED)
if(CURL_FOUND)
    message(STATUS "find curl library, include directory: " ${CURL_INCLUDE_DIRS})
else(CURL_FOUND)
    message(FATAL_ERROR "Can not find curl library")
endif(CURL_FOUND)

#
aux_source_directory(${CMAKE_SOURCE_DIR}/third_party/mongoose/ MONGOOSE_SRC_FILES)

#
aux_source_directory(${CMAKE_SOURCE_DIR}/any/ SRC_FILES)

add_executable(any_unit ${SRC_FILES} ${MONGOOSE_SRC_FILES})

target_include_directories(any_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                           ${CMAKE_SOURCE_DIR}/include/ 
                                           ${CMAKE_SOURCE_DIR}/third_party/ 
                                           ${OpenCV_INCLUDE_DIRS}
                          )

target_link_libraries(any_unit glog::glog 
                               gflags 
                               yaml-cpp::yaml-cpp 
                               Boost::filesystem 
                               OpenSSL::SSL 
                               OpenSSL::Crypto 
                               PkgConfig::ffmpeg 
                               ${OpenCV_LIBS}
                               CURL::libcurl
                     )

#
aux_source_directory(${CMAKE_SOURCE_DIR}/net/ NETWORK_SRC_FILES)

add_executable(net_unit ${NETWORK_SRC_FILES} ${MONGOOSE_SRC_FILES})

target_include_directories(net_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                           ${CMAKE_SOURCE_DIR}/include/ 
                                           ${CMAKE_SOURCE_DIR}/third_party/ 
                                           ${OpenCV_INCLUDE_DIRS}
                          )

target_link_libraries(net_unit glog::glog 
                               gflags 
                               yaml-cpp::yaml-cpp 
                               Boost::filesystem 
                               OpenSSL::SSL 
                               OpenSSL::Crypto 
                               PkgConfig::ffmpeg 
                               ${OpenCV_LIBS}
                               CURL::libcurl
                     )

#
aux_source_directory(${CMAKE_SOURCE_DIR}/ffmpeg/ FFMPEG_SRC_FILES)

add_executable(ffmpeg_unit ${FFMPEG_SRC_FILES} ${MONGOOSE_SRC_FILES})

target_include_directories(ffmpeg_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                              ${CMAKE_SOURCE_DIR}/include/ 
                                              ${CMAKE_SOURCE_DIR}/third_party/ 
                                              ${OpenCV_INCLUDE_DIRS}
                          )

target_link_libraries(ffmpeg_unit glog::glog 
                                  gflags 
                                  yaml-cpp::yaml-cpp 
                                  Boost::filesystem 
                                  OpenSSL::SSL 
                                  OpenSSL::Crypto 
                                  PkgConfig::ffmpeg 
                                  ${OpenCV_LIBS}
                                  CURL::libcurl
                     )


get_target_property(TARGET_SOURCES ffmpeg_unit SOURCES)
message(STATUS "ffmpeg target sources: ${TARGET_SOURCES}")

get_target_property(TARGET_INCLUDE_DIRECTORIES ffmpeg_unit INCLUDE_DIRECTORIES)
message(STATUS "ffmpeg target include directories: ${TARGET_INCLUDE_DIRECTORIES}")