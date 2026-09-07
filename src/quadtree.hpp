#ifndef QUADTREE_H 
#define QUADTREE_H
class quadNode {
	public: 
		double x;
		double y;
		double size_x;
		double size_y;
		quadNode* parent;
		quadNode* children;

		double sx, sy, smass;

		int count;

		quadNode(double x, double y, double size_x, double size_y, quadNode* parent);

		void divide();
		quadNode* divide(double x, double y); //completely different behaviour btw #trolled
		void clear();
};
class quadTree {
	public:
		quadNode* root;
		quadTree(double orig, double size);
		void clear();
		void clear(double orig, double size); //no non-squares allowed
};
	
#endif
