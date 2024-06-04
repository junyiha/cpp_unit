#
message(STATUS "any sub project")

#
aux_source_directory(${CMAKE_SOURCE_DIR}/any/ SRC_FILES)

# add_executable(any_unit ${SRC_FILES} ${MONGOOSE_SRC_FILES})
add_executable(any_unit ${SRC_FILES} )

target_include_directories(any_unit PUBLIC ${Boost_INCLUDE_DIR} 
                                           ${CMAKE_SOURCE_DIR}/include/ 
                                           ${CMAKE_SOURCE_DIR}/third_party/ 
                                           ${OpenCV_INCLUDE_DIRS}
                          )

target_link_libraries(any_unit glog::glog 
                               gflags 
                               yaml-cpp::yaml-cpp 
                               Boost::filesystem 
                               Boost::timer 
                               OpenSSL::SSL 
                               OpenSSL::Crypto 
                               PkgConfig::ffmpeg 
                               ${OpenCV_LIBS}
                               CURL::libcurl
                     )

get_target_property(TARGET_SOURCES any_unit SOURCES)
message(STATUS "anything target sources: ${TARGET_SOURCES}")

get_target_property(TARGET_INCLUDE_DIRECTORIES any_unit INCLUDE_DIRECTORIES)
message(STATUS "anything target include directories: ${TARGET_INCLUDE_DIRECTORIES}")

get_target_property(TARGET_LINK_LIBRARIES any_unit LINK_LIBRARIES)
message(STATUS "anything target include directories: ${TARGET_LINK_LIBRARIES}")