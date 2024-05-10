#
message(STATUS "letcode sub project")

#
aux_source_directory(${CMAKE_SOURCE_DIR}/leet_code/ LEET_CODE_SRC_FILES)

add_executable(leet_code_unit ${LEET_CODE_SRC_FILES})

target_include_directories(leet_code_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                                 ${CMAKE_SOURCE_DIR}/include/ 
                                                 ${CMAKE_SOURCE_DIR}/third_party/ 
                                                 ${OpenCV_INCLUDE_DIRS}
                          )

target_link_libraries(leet_code_unit glog::glog 
                                     gflags 
                                     yaml-cpp::yaml-cpp 
                                     Boost::filesystem 
                                     OpenSSL::SSL 
                                     OpenSSL::Crypto 
                                     PkgConfig::ffmpeg 
                                     ${OpenCV_LIBS}
                                     CURL::libcurl
                                     PkgConfig::zmq
                     )

get_target_property(TARGET_SOURCES leet_code_unit SOURCES)
message(STATUS "leet_code_unit target sources: ${TARGET_SOURCES}")

get_target_property(TARGET_INCLUDE_DIRECTORIES leet_code_unit INCLUDE_DIRECTORIES)
message(STATUS "leet_code_unit target include directories: ${TARGET_INCLUDE_DIRECTORIES}")

get_target_property(TARGET_LINK_LIBRARIES leet_code_unit LINK_LIBRARIES)
message(STATUS "leet_code_unit target include directories: ${TARGET_LINK_LIBRARIES}")                     