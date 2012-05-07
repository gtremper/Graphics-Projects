/* Light.cpp: Defines methods for the light class */
#include <vector>
#include <math.h>
#include <iostream>

#include "Shapes.h"
#include "Intersection.h"
#include "Light.h"

#define EPSILON 0.00000001

using namespace std;


/***  Directional Light  ***/
DirectionalLight::DirectionalLight(const vec3& c,const vec3& dir) {
	color = c;
	direction = glm::normalize(dir);
}

vec3 DirectionalLight::shade(const Intersection& hit, TreeNode& tree, const vec3& normal){
	if(!isVisible(hit.point+EPSILON*normal,tree)) {
		return vec3(0.0,0.0,0.0);
	}
	vec3 shade = max(0.0,glm::dot(normal,direction)) * hit.primative->diffuse;
	vec3 half = glm::normalize(hit.sourceDirection+direction);
	double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
	shade += phong * hit.primative->specular;
	shade *= color;
	
	return shade;
}

bool DirectionalLight::isVisible(const vec3& point, TreeNode& tree) {
	/*
	Ray ray(point,direction);
	vector<Shape*>::const_iterator prim=objects.begin();
	for(;prim!=objects.end(); ++prim){
		if ((*prim)->intersect(ray) >= 0.0){
			return false;
		}
	}
	return true;
	*/
	Ray ray(point,direction);
	Intersection hit = tree.intersect(ray);
	return hit.primative;
}

/***  Point Light  ***/
PointLight::PointLight(const vec3& colour,const vec3& poi, double con, double lin, double quad) {
	color = colour;
	point = poi;
	constant = con;
	linear = lin;
	quadratic = quad;
}

vec3 PointLight::shade(const Intersection& hit, TreeNode& tree, const vec3& normal){
	if( !isVisible(hit.point+EPSILON*normal,tree) ){
		return vec3(0.0,0.0,0.0);
	}
	vec3 direction = glm::normalize(point-hit.point);
	vec3 shade = max(0.0,glm::dot(normal,direction)) * hit.primative->diffuse;
	
	vec3 half = glm::normalize(hit.sourceDirection+direction);
	double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
	shade += phong * hit.primative->specular;
	shade *= color;
	double dist = glm::distance(point,hit.point);
	shade *= 1.0/(constant + linear*dist + quadratic*dist*dist);
	return shade;
}

bool PointLight::isVisible(const vec3& p, TreeNode& tree) {
	/*
	vec3 direction = glm::normalize(point-p);
	Ray ray(p,direction);
	vector<Shape*>::const_iterator prim=objects.begin();
	double dist = glm::distance(point,p);
	for(;prim!=objects.end(); ++prim){
		double t = (*prim)->intersect(ray);
		if (t >= 0.0 && t<dist ) return false;
	}
	return true;
	*/
	vec3 direction = glm::normalize(point-p);
	Ray ray(p,direction);
	double dist = glm::distance(point,p);
	Intersection hit = tree.intersect(ray);
	if(hit.primative){
		if(glm::distance(p,hit.point) > dist){
			return true;
		}
	}
	return false;
}

