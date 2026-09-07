#include <stdlib.h>
#include <stdio.h>
#include "quadtree.hpp"

#define WIDTH 800
#define HEIGHT 600

quadNode::quadNode(double x, double y, double size_x, double size_y, int parent, int index) {
	this->x = x;
	this->y = y;
	this->size_x = size_x;

	this->parent = parent;
	this->children = -1;
	this->self = index;

	this->smass = 0;
	this->sx = 0;
	this->sy = 0;
	this->count = 0;

}


int quadNode::divide(quadTree& tree, double x, double y){
	//Our goal, our mission statement, is the creation of a subdivision that includes point (x,y)
	//printf("%d %lu\n", self, tree.tree.size());
	//printf("%d\n", self);
	int quad = self;
	//printf("%p\n", quad);
	while(tree.tree[quad].children != -1){
		//printf("%d\n", quad.children);
		double cx = tree.tree[quad].x + tree.tree[quad].size_x/2;
		double cy = tree.tree[quad].y + tree.tree[quad].size_x/2;
		int i = 0;
		
		if(cx < x){
			i += 2;
		}
		if(cy < y){
			i += 1;
		}
		//printf("%d\n", i);
		//le sigh we cringe on
		quad = tree.tree[quad].children+i;
	}
	//printf("%p\n", quad);
	//printf("%lu -> ", tree.size());
	tree.divide(quad);
	//printf("%lu\n", tree.size());
	double cx = tree.tree[quad].x + tree.tree[quad].size_x/2;
	double cy = tree.tree[quad].y + tree.tree[quad].size_x/2;
	int i = 0;
	
	if(cx < x){
		i += 2;
	}
	if(cy < y){
		i += 1;
	}
	return tree.tree[quad].children+i;
}



quadTree::quadTree(double orig, double size) {
	printf("Initializing quad tree\n");
	tree.reserve(100);
	tree.push_back(quadNode(orig, orig, size, size, -1, 0));
	root = 0;
	printf("Initialized quad tree\n");
}

void quadTree::clear(){
	double x = tree[root].x;
	double y = tree[root].y;
	double width = tree[root].size_x;
	tree.clear();
	tree.push_back(quadNode(x, y, width, width, -1, 0));
}
void quadTree::clear(double orig, double size){
	//printf("Clearing the qtree\n");
	tree.clear();
	tree.push_back(quadNode(orig, orig, size, size, -1, 0));
	//printf("%lu\n", tree.size());
	//printf("Finished Clearing the qtree\n");
}


size_t quadTree::size(){
	return tree.size();
}

void quadTree::push_back(quadNode& node){
	tree.push_back(node);
}

void quadTree::divide(int index){
	double x = tree[index].x; double y = tree[index].y;
	double size_x = tree[index].size_x; double size_y = tree[index].size_x; 
	int self = tree[index].self; int children = tree.size();

	for(int ix=0; ix<=1; ix++){
		for(int iy=0; iy<=1; iy++){
			tree.push_back(quadNode(x + ix * size_x/2, y + iy * size_y/2, size_x/2, size_y/2, self, children+ix*2+iy)); //this language sucks
		}
	}
	tree[index].children = children;
}

void quadTree::propogate(){
	for(int i=(int)tree.size()-1; i>=1; i--){
		quadNode& n = tree[i];
		quadNode& p = tree[n.parent];
		p.sx += n.sx; p.sy += n.sy;
		p.smass += n.smass; p.count += n.count;
	}
}
