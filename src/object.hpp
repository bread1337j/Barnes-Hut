#ifndef OBJECT_H
#define OBJECT_H
#include "quadtree.hpp"
class object {
	public:
		double x, y, mass;
		double vx, vy;
		quadNode* node;
		object(double x, double y, double mass);	
		object(double x, double y);	
		void grabNode(quadTree* tree);
		void propogate();
		void doGrav(quadNode* cluster, double dt);
		void gravTick(quadNode* root, double dt);
		void physTick(double dt);
		void refine();
};

#endif
