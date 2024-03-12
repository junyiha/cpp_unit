#
cmake_minimum_required(VERSION 3.20)

#
project(model_det)

#
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

#
set(cuda_header "/usr/local/cuda-11.8/targets/x86_64-linux/include/")
set(cuda_path "/usr/local/cuda-11.8/targets/x86_64-linux/lib/")

#
set(tensorrt_header "/usr/local/TensorRT-8.5.1.7/include/")
set(tensorrt_path "/usr/local/TensorRT-8.5.1.7/lib/")
set(tensorrt2_path "/usr/local/TensorRT-8.5.1.7/targets/x86_64-linux-gnu/lib/")

#
aux_source_directory(${CMAKE_SOURCE_DIR}/ SRC)

add_executable(model_det ${SRC})

target_include_directories(model_det PRIVATE ${cuda_header} ${tensorrt_header})

target_link_directories(model_det PRIVATE ${cuda_path} ${tensorrt_path} ${tensorrt2_path})

target_link_libraries(model_det cudart nvinfer)