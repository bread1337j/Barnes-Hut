#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include "multiprocessing.h"


__global__ void helloworld(){
	printf("Hello from thread %d\n", threadIdx.x);
	return;
}

extern "C++" void nothing(){
	helloworld<<<1, 256>>>();
}

extern "C++" cudaState* initCudaState(){
	cudaState* out = (cudaState*) malloc(sizeof(cudaState)); //malloc so that if I forget to init something I notice
	
	out->objarr = NULL;
	out->objCount = 0;

	return out;
}

__device__ void doGrav(object* objarr, int pos, quadNode* tree, int nodePos, double dt){

}


#define THRESHOLD 1
__global__ void gravTick(object* objarr, int pos, quadNode* tree, int nodePos, double dt){
	nodePos += threadIdx.x;
	quadNode node = tree[nodePos];
	if(node.count == 0) {
		return;
	}
	if(node.children == -1 || node.count == 1) { doGrav(objarr, pos, tree, nodePos, dt); return; }

	double dx = (node.sx/node.smass) - objarr[pos].x;
	double dy = (node.sy/node.smass) - objarr[pos].y;
	double ratio = node.size_x / sqrt(dx*dx + dy*dy);

	if(ratio < THRESHOLD){
		doGrav(objarr, pos, tree, nodePos, dt);
	}else{
		gravTick<<<1, 4>>>(objarr, pos, tree, node.children, dt);
	}
}

__global__ void drift(object* objarr, int pos, quadNode* tree, double dt){
	objarr[pos].ax = 0;
	objarr[pos].ay = 0;

	gravTick<<<1, 1>>>(objarr, pos, tree, 0, dt);
}

extern "C++" void doObjectDrift(cudaState* state, std::vector<object> arr, const quadTree* tree){
	int blockSize = 256;
	int blockNum = (arr.size() + blockSize-1) / (blockSize);

	if(arr.size() != state->objCount){
		state->objCount = arr.size();
		cudaFree(state->objarr);
		cudaMalloc(&(state->objarr), state->objCount * sizeof(object));
	}
					//not fond of what these vectors are doing....
	cudaMemcpy(state->objarr, &arr[0], state->objCount, cudaMemcpyHostToDevice);

	state->quadCount = tree->tree.size();
	cudaFree(state->quadTree);
	cudaMalloc(&(state->quadTree), state->quadCount * sizeof(quadNode));
	cudaMemcpy(state->quadTree, &tree->tree[0], state->quadCount, cudaMemcpyHostToDevice);

	
	

	cudaMemcpy(&arr[0], state->objarr, state->objCount, cudaMemcpyDeviceToHost);
}




