#include <cuda_runtime.h>
#include <device_launch_parameters.h>

__global__ void nothing(){
	printf("Hello from thread %d\n", threadIdx.x);
	return;
}
