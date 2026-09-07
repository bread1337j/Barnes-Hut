#include <stdlib.h>
#include <stdio.h>
#include "quadtree.hpp"

#define WIDTH 800
#define HEIGHT 600

quadNode::quadNode(double x, double y, double size_x, double size_y, quadNode* parent) {
	this->x = x;
	this->y = y;
	this->size_x = size_x;
	this->size_y = size_y;
	this->parent = parent;
	this->children = NULL;
	this->smass = 0;
	this->sx = 0;
	this->sy = 0;
	this->count = 0;
}

void quadNode::divide(){
	children = (quadNode*) malloc(sizeof(quadNode) * 4);
	for(int ix=0; ix<=1; ix++){
		for(int iy=0; iy<=1; iy++){
			children[ix * 2 + iy] = quadNode(x + ix * size_x/2, y + iy * size_y/2, size_x/2, size_y/2, this);
		}
	}
}

quadNode* quadNode::divide(double x, double y){
	//Our goal, our mission statement, is the creation of a subdivision that includes point (x,y)
	quadNode* quad = this;
	//printf("%p\n", quad);
	while(quad->children != NULL){
		double cx = quad->x + quad->size_x/2;
		double cy = quad->y + quad->size_y/2;
		int i = 0;
		
		if(cx < x){
			i += 2;
		}
		if(cy < y){
			i += 1;
		}
		//printf("%d\n", i);
		quad = (quad->children+i);
	}
	//printf("%p\n", quad);
	quad->divide();
	double cx = quad->x + quad->size_x/2;
	double cy = quad->y + quad->size_y/2;
	int i = 0;
	
	if(cx < x){
		i += 2;
	}
	if(cy < y){
		i += 1;
	}
	return quad->children+i;
}

void quadNode::clear() {
	if(this->children != NULL){
		for(int i=0; i<4; i++){
			(this->children+i)->clear();
		}
		free(this->children);
		this->children = NULL;
	}
}


quadTree::quadTree(double orig, double size) {
	root = new quadNode(orig, orig, size, size, NULL);
}

void quadTree::clear(){
	root->clear();
	double x = root->x;
	double y = root->y;
	double width = root->size_x;
	double height = root->size_y;
	delete root;
	root = new quadNode(x, y, width, height, NULL);
}
void quadTree::clear(double orig, double size){
	root->clear();
	delete root;
	root = new quadNode(orig, orig, size, size, NULL);
}
