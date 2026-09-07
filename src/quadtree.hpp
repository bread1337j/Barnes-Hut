#ifndef QUADTREE_H 
#define QUADTREE_H
#include <vector>

class quadTree;

class quadNode {
	public: 
		double x;
		double y;
		double size_x;

		int parent;
		int children;
		int self;
		int count;

		double sx, sy, smass;


		quadNode(double x, double y, double size_x, double size_y, int parent, int index);

		int divide(quadTree& tree, double x, double y); //completely different behaviour btw #trolled
};
class quadTree {
	public:
		std::vector<quadNode> tree;
		int root;
		quadTree(double orig, double size);
		void clear();
		void clear(double orig, double size); //no non-squares allowed
		quadNode& access(int index){
			return tree[index];
		}
		void divide(int index);
		size_t size();
		void push_back(quadNode& node);
		void propogate();
};
	
#endif
