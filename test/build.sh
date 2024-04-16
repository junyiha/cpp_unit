#! /bin/bash -x

# g++ test.cpp -o test.exe -lcurl

g++ onnx_test.cpp common/logger.cpp -o onnx_test.exe -I./common/ -I/usr/local/TensorRT-8.5.1.7/include/ -I/usr/local/cuda/include/ -L/usr/local/cuda/lib64 -L/usr/local/TensorRT-8.5.1.7/lib/ -lnvinfer -lcudart