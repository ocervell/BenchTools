#! /bin/bash
nvcc -c gpuops.cu
mpicc -L/usr/local/cuda/include -L/usr/local/cuda/lib64 -lcudart -ogpu gpu.cpp gpuops.o
