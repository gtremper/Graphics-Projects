/* Implementation of KDTree acceleration structure */

#include <iostream>
#include <vector>

#include "Shapes.h"
#include "Intersection.h"
#include "KDTree.h"

using namespace std;

#define TREELIMIT 5

/***  TREENODE  ***/
struct ShapeSorter {
	ShapeSorter(int ax){this->axis = ax;}
	bool operator() (Shape* i, Shape* j) {
		return i->aabb.center[axis] < j->aabb.center[axis];
	}
	int axis;
};

TreeNode::TreeNode(vector<Shape*>& prims, int axis, AABB bigBox){
	this->aabb = bigBox;
	if(prims.size()<=TREELIMIT){
		left = NULL;
		right = NULL;
		primatives = prims;
	}else{
		//primatives = NULL;
		ShapeSorter s(axis);
		sort(prims.begin(), prims.end(),s);
		double split = prims[prims.size()/2]->aabb.center[axis];
		
		vector<Shape*> leftPrims;
		vector<Shape*>::iterator it=prims.begin();
		for(;it!=prims.end(); ++it){
			if((*it)->aabb.bounds[2*axis]<split){
				leftPrims.push_back(*it);
			}
		}
		
		vector<Shape*> rightPrims;
		for(it=prims.begin(); it!=prims.end(); ++it){
			if((*it)->aabb.bounds[2*axis+1]>split){
				rightPrims.push_back(*it);
			}
		}
		
		AABB leftAABB = AABB(bigBox);
		leftAABB.bounds[2*axis+1] = split;
		
		AABB rightAABB = AABB(bigBox);
		rightAABB.bounds[2*axis] = split;
		
		int newAxis = (axis+1)%3;
		left = new TreeNode(leftPrims, newAxis, leftAABB);
		right = new TreeNode(rightPrims, newAxis, rightAABB);	
	}
}

TreeNode::~TreeNode(){
	if(left){
		delete left;
		delete right;
	}
}

Intersection TreeNode::intersect(Ray& ray){
	return Intersection(primatives,ray);
}



