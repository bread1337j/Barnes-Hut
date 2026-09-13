#ifndef MULTIPROCESSING_H
#define MULTIPROCESSING_H
#include "object.hpp"
#include "quadtree.hpp"
#include <stdlib.h>
#include <stdio.h>

struct cudaState {
	object* objarr;
	int objCount;

	quadNode* quadTree;
	int quadCount;
};

void nothing();
cudaState* initCudaState();
void doObjectDrift(cudaState* state, std::vector<object> arr, const quadTree* tree);
#endif
