#include <stdio.h>

__global__ void helloFromGPU (void)
{
    printf("Hello world from GPU!\n");
}

int main()
{
    printf("Hello world from CPU!\n");

    helloFromGPU <<<10, 10>>>();
    cudaDeviceReset();

    return 0;
}