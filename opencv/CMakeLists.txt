#
message(STATUS "opencv sub project")

#
aux_source_directory(${CMAKE_SOURCE_DIR}/opencv/ OPENCV_SRC_FILES)

add_executable(opencv_unit ${OPENCV_SRC_FILES} ${MONGOOSE_SRC_FILES})

target_include_directories(opencv_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                              ${CMAKE_SOURCE_DIR}/include/ 
                                              ${CMAKE_SOURCE_DIR}/third_party/ 
                                              ${OpenCV_INCLUDE_DIRS}
                          )

target_link_libraries(opencv_unit glog::glog 
                                  gflags 
                                  yaml-cpp::yaml-cpp 
                                  Boost::filesystem 
                                  OpenSSL::SSL 
                                  OpenSSL::Crypto 
                                  PkgConfig::ffmpeg 
                                  ${OpenCV_LIBS}
                                  CURL::libcurl
                     )

get_target_property(TARGET_SOURCES opencv_unit SOURCES)
message(STATUS "opencv target sources: ${TARGET_SOURCES}")

get_target_property(TARGET_INCLUDE_DIRECTORIES opencv_unit INCLUDE_DIRECTORIES)
message(STATUS "opencv target include directories: ${TARGET_INCLUDE_DIRECTORIES}")

get_target_property(TARGET_LINK_LIBRARIES opencv_unit LINK_LIBRARIES)
message(STATUS "opencv target include directories: ${TARGET_LINK_LIBRARIES}")