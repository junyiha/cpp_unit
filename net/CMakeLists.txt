#
message(STATUS "net sub project")

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
                               PkgConfig::zmq
                     )

get_target_property(TARGET_SOURCES net_unit SOURCES)
message(STATUS "network target sources: ${TARGET_SOURCES}")

get_target_property(TARGET_INCLUDE_DIRECTORIES net_unit INCLUDE_DIRECTORIES)
message(STATUS "network target include directories: ${TARGET_INCLUDE_DIRECTORIES}")

get_target_property(TARGET_LINK_LIBRARIES net_unit LINK_LIBRARIES)
message(STATUS "network target include directories: ${TARGET_LINK_LIBRARIES}")

