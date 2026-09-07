#ifndef OBJECT_H
#define OBJECT_H
#include "quadtree.hpp"
class object {
	public:
		double x, y, mass;
		double vx, vy;
		int node;
		object(double x, double y, double mass);	
		object(double x, double y);	
		void grabNode(quadTree& tree);
		void propogate(quadTree& tree);
		void doGrav(quadTree& tree, int cluster, double dt);
		void gravTick(quadTree& tree, int node, double dt);
		void physTick(double dt);
		void refine(quadTree& tree);
};

#endif
