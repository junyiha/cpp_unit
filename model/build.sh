#! /bin/bash -x

g++ yolov5_det.cpp -o yolov5_det -I/usr/local/cuda-11.8/targets/x86_64-linux/include/ -L/usr/local/cuda-11.8/targets/x86_64-linux/lib/ -lcudart