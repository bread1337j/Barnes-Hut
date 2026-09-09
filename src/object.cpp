#include "object.hpp"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
 
object::object(double x, double y, double mass){
	this->x = x;
	this->y = y;
	this->mass = mass;
	this->node = -1;
	this->vx = 0; this->vy = 0;
	this->ax = 0; this->ay = 0;
}


object::object(double x, double y){
	this->x = x;
	this->y = y;
	this->mass = 1;
	this->node = -1;
	this->vx = 0; this->vy = 0;
	this->vx = 0; this->vy = 0;
	this->ax = 0; this->ay = 0;
}

void object::grabNode(quadTree& tree){
	//printf("Grabbing a node from a tree of size %lu\n", tree.size());
	this->node = tree.access(tree.root).divide(tree, this->x, this->y);
}

void object::propogate(quadTree& tree){
	if(this->node == -1){
		return;
	}
	int node = this->node;
	tree.access(node).sx += this->x * this->mass; 
	tree.access(node).sy += this->y * this->mass;
	tree.access(node).smass += this->mass;
	tree.access(node).count += 1;
}

void object::refine(quadTree& tree){
	if(this->node == -1){
		return;
	}
	while(tree.access(node).children != -1){
		double cx = tree.access(node).x + tree.access(node).size_x/2;
		double cy = tree.access(node).y + tree.access(node).size_x/2;
		int i = 0;
		if(this->x > cx) i += 2;
		if(this->y > cy) i += 1;
		this->node = tree.access(node).children+i;
	}
}

#define EPSILON 4
void object::doGrav(quadTree& tree, int cluster, double dt){
	double G = 6.67e-9;
	//printf("Release me\n");
	double dx = tree.access(cluster).sx/(tree.access(cluster).smass) - this->x; 
	double dy = tree.access(cluster).sy/(tree.access(cluster).smass) - this->y;

	double dmagsq = dx*dx+dy*dy+EPSILON*EPSILON;

	double forceMag = (G * tree.access(cluster).smass / (dmagsq*sqrt(dmagsq)));
	//printf("aaa\n");


	this->ax += dx*forceMag;
	this->ay += dy*forceMag; 

}

void object::gravTick(quadTree& tree, int index, double dt){


	double threshold = 0.65;	


	//printf("what\n");
	quadNode& node = tree.access(index);
	//printf("what2\n");
	if(node.count == 0){
		return;
	}
	if(node.children == -1 || node.count == 1) { doGrav(tree, index, dt); return; }


	//printf("what3\n");
	double dx = node.sx/(node.smass) - this->x; 
	double dy = node.sy/(node.smass) - this->y;
	double ratio = node.size_x / sqrt(dx*dx + dy*dy);
	//printf("what4\n");
	if(ratio < threshold){
		doGrav(tree, index, dt);
		//printf("what5\n");
	}else{
		//printf("what6 %d\n", node.children);
		for(int i=0; i<4; i++){
			gravTick(tree, node.children+i, dt);
		}
		//printf("what7\n");
	}
	//printf("que\n");

}


void object::kick(double dt){
	this->vx += this->ax*0.5 * dt;
	this->vy += this->ay*0.5 * dt;
}

void object::drift(quadTree& tree, double dt){

	this->ax = 0; this->ay = 0;
	//what not thinking to use a vector does to a man...C brain and its consequences or something...
	gravTick(tree, 0, dt);

}

void object::move(double dt){
	this->x += this->vx * dt;
	this->y += this->vy * dt;

}
