// src/heart.cu

#include <iostream>

__global__ void hello_from_gpu() {
    printf("Hello from GPU thread %d!\n", threadIdx.x);
}

void launch_cuda_kernel() {
    hello_from_gpu<<<1, 5>>>();
    cudaDeviceSynchronize();
}
