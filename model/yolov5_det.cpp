/**
 * @file yolov5_det.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>

#include <cuda_runtime.h>


int main(int argc, char *argv[])
{
    auto cuda_status = cudaSetDevice(0);

    if (cuda_status != cudaSuccess)
    {
        std::cerr << "cudaSetDevice failed, Error: " << cudaGetErrorString(cuda_status) << std::endl;
        return -1;
    }

    std::cerr << "quit...\n";
    return 0;
}