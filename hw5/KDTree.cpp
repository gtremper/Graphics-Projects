/* Implementation of KDTree acceleration structure */

#include <iostream>
#include <vector>

#include "Shapes.h"
#include "Intersection.h"
#include "KDTree.h"

using namespace std;

#define TREELIMIT 5
#define EPSILON 0.000001

/***  TREENODE  ***/
struct ShapeSorter {
	ShapeSorter(int ax){this->axis = ax;}
	bool operator() (Shape* i, Shape* j) {
		return i->aabb.center[axis] < j->aabb.center[axis];
	}
	int axis;
};

TreeNode::TreeNode(vector<Shape*>& prims, int ax, AABB& bigBox){
	//cout<<"axis: "<< ax <<endl;
	this->aabb = bigBox;
	this->axis = ax;
	
	ShapeSorter s(axis);
	sort(prims.begin(), prims.end(),s);
	this->split = prims[prims.size()/2]->aabb.center[axis];
	
	vector<Shape*> leftPrims;
	vector<Shape*>::iterator it=prims.begin();
	for(;it!=prims.end(); ++it){
		if((*it)->aabb.bounds[2*axis]<(split)){
			leftPrims.push_back(*it);
		}
	}
	
	vector<Shape*> rightPrims;
	for(it=prims.begin(); it!=prims.end(); ++it){
		if((*it)->aabb.bounds[2*axis+1]>(split)){
			rightPrims.push_back(*it);
		}
	}
	
	cout << "LEFTSIZE: " <<leftPrims.size()<<endl;
	cout << "RIGHTSIZE: " <<rightPrims.size()<<endl;
	//if(prims.size()<=TREELIMIT){
	if(leftPrims.size()==prims.size() || rightPrims.size()==prims.size() || prims.size()<=TREELIMIT){
		left = NULL;
		right = NULL;
		primatives = prims;
		cout << "LEAFNODE: "<<aabb.bounds[0]<<" "<<aabb.bounds[1]<<" "<<aabb.bounds[2]<<" "<<aabb.bounds[3]<<" "<<aabb.bounds[4]<<" "<<aabb.bounds[5]<<endl;
		cout << "Size: "<<prims.size()<<endl;
	}else{
		AABB leftAABB = AABB(aabb.bounds[0],aabb.bounds[1],aabb.bounds[2],aabb.bounds[3],aabb.bounds[4],aabb.bounds[5]);
		leftAABB.bounds[2*axis+1] = split;
		
		AABB rightAABB = AABB(aabb.bounds[0],aabb.bounds[1],aabb.bounds[2],aabb.bounds[3],aabb.bounds[4],aabb.bounds[5]);
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
	if(!left){
		//cout << "HIT BOX"<<endl;
		return Intersection(primatives,ray);
	}
	if(ray.origin[axis]<(split)){
		if(left->aabb.intersect(ray)){
			//cout <<"1st left"<<endl;
			Intersection hit = left->intersect(ray);
			if(hit.primative) return hit;
		}
		if(right->aabb.intersect(ray)){
			Intersection hit = right->intersect(ray);
			if(hit.primative) return hit;
		}
	} else {
		if(right->aabb.intersect(ray)){
			//cout <<"2nd right"<<endl;
			Intersection hit = right->intersect(ray);
			if(hit.primative) return hit;
		}
		if(left->aabb.intersect(ray)){
			//cout << "2nd left"<<endl;
			Intersection hit = left->intersect(ray);
			if(hit.primative) return hit;
		}
	}
	return Intersection();
}



