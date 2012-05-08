/* Class definition for lights */
#ifndef LIGHT_H
#define LIGHT_H
#include "KDTree.h"

class Light {
public:
	virtual ~Light(){};
	virtual vec3 shade(const Intersection& , TreeNode& tree,const vec3&)=0;	
	vec3 color;
};

class DirectionalLight : public Light {
public:
	~DirectionalLight(){};
	DirectionalLight(const vec3& color,const vec3& dir);
	vec3 shade(const Intersection& hit,  TreeNode& tree,const vec3&);	
	bool isVisible(const vec3& point,  TreeNode& tree);
	vec3 direction;
};

class PointLight : public Light {
public:
	~PointLight(){};
	PointLight(const vec3& color,const vec3& p, double,double,double);
	vec3 shade(const Intersection& hit, TreeNode& tree,const vec3&);	
	bool isVisible(const vec3& point,  TreeNode& tree);
	vec3 point;
	double constant;
	double linear;
	double quadratic;
	double radius;
	int shadowRays;
};

#endif