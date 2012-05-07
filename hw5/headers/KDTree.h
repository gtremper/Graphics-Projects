/* Class definitions for KDTree acceleration structure */

#ifndef KDTREE_H
#define KDTREE_H

class TreeNode {
public:
	TreeNode(std::vector<Shape*>&,int,AABB&,bool);
	~TreeNode();
	Intersection intersect(Ray&);
	TreeNode* left;
	TreeNode* right;
	std::vector<Shape*> primatives;
	AABB aabb;
	int axis;
	double split;
};

#endif

