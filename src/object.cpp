#include "object.hpp"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
 
object::object(double x, double y, double mass){
	this->x = x;
	this->y = y;
	this->mass = mass;
	this->node = nullptr;
	this->vx = 0; this->vy = 0;
}


object::object(double x, double y){
	this->x = x;
	this->y = y;
	this->mass = 1;
	this->node = nullptr;
	this->vx = 0; this->vy = 0;
}

void object::grabNode(quadTree* tree){
	this->node = tree->root->divide(this->x, this->y);
}

void object::propogate(){
	if(this->node == nullptr){
		return;
	}
	quadNode* node = this->node;
	node->sx += this->x * this->mass; 
	node->sy += this->y * this->mass;
	node->smass += this->mass;
	node->count += 1;
	while(node->parent != nullptr){
		node->parent->sx += this->x * this->mass;
		node->parent->sy += this->y * this->mass;
		node->parent->smass += this->mass;
		node->parent->count += 1;
		node = node->parent;
	}
}

void object::refine(){
	if(this->node == nullptr){
		return;
	}
	while(this->node->children != nullptr){
		double cx = this->node->x + this->node->size_x/2;
		double cy = this->node->y + this->node->size_y/2;
		int i = 0;
		if(this->x > cx) i += 2;
		if(this->y > cy) i += 1;
		this->node = this->node->children+i;
	}
}

#define EPSILON 4
void object::doGrav(quadNode* cluster, double dt){
	double G = 6.67e-9;

	double dx = cluster->sx/(cluster->smass) - this->x; 
	double dy = cluster->sy/(cluster->smass) - this->y;

	double dmagsq = dx*dx+dy*dy+EPSILON*EPSILON;

	double forceMag = G * cluster->smass / (dmagsq*sqrt(dmagsq));
	
	this->vx += dx*forceMag*dt;
	this->vy += dy*forceMag*dt; 

}

void object::gravTick(quadNode* node, double dt){
	double threshold = 0.5;	
	if(node->count == 0){
		return;
	}
	double dx = node->sx/(node->smass) - this->x; 
	double dy = node->sy/(node->smass) - this->y;
	double ratio = node->size_x / sqrt(dx*dx + dy*dy);
	if(node->children == NULL || ratio < threshold || node->count == 1){
		doGrav(node, dt);
	}else{
		for(int i=0; i<4; i++){
			gravTick(node->children+i, dt);
		}
	}

}

void object::physTick(double dt){
	this->x += vx * dt;
	this->y += vy * dt;
}
