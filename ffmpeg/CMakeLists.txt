#
message(STATUS "ffmpeg sub project")

#
aux_source_directory(${CMAKE_SOURCE_DIR}/ffmpeg/ FFMPEG_SRC_FILES)

add_executable(ffmpeg_unit ${FFMPEG_SRC_FILES} ${MONGOOSE_SRC_FILES})

target_include_directories(ffmpeg_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                              ${CMAKE_SOURCE_DIR}/include/ 
                                              ${CMAKE_SOURCE_DIR}/third_party/ 
                                              ${CMAKE_CURRENT_SOURCE_DIR}/
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
                                  dlib::dlib
                     )

get_target_property(TARGET_SOURCES ffmpeg_unit SOURCES)
message(STATUS "ffmpeg target sources: ${TARGET_SOURCES}")

get_target_property(TARGET_INCLUDE_DIRECTORIES ffmpeg_unit INCLUDE_DIRECTORIES)
message(STATUS "ffmpeg target include directories: ${TARGET_INCLUDE_DIRECTORIES}")

get_target_property(TARGET_LINK_LIBRARIES ffmpeg_unit LINK_LIBRARIES)
message(STATUS "ffmpeg target include directories: ${TARGET_LINK_LIBRARIES}")
