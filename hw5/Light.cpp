/* Light.cpp: Defines methods for the light class */
#include <vector>
#include <math.h>
#include <iostream>

#include "Shapes.h"
#include "Intersection.h"
#include "Light.h"

#define EPSILON 0.005

using namespace std;


/***  Directional Light  ***/
DirectionalLight::DirectionalLight(const vec3& c,const vec3& dir) {
	color = c;
	direction = glm::normalize(dir);
}

vec3 DirectionalLight::shade(const Intersection& hit, const vector<Shape*>& objects, const vec3& normal){
	if(!isVisible(hit.point,objects)) {
		return vec3(0.0,0.0,0.0);
	}
	vec3 shade = max(0.0,glm::dot(normal,-direction)) * hit.primative->diffuse;
	vec3 half = glm::normalize(hit.sourceDirection-direction);
	double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
	shade += phong * hit.primative->specular;
	shade *= color;
	
	return shade;
}

bool DirectionalLight::isVisible(const vec3& point, const vector<Shape*>& objects) {
	Ray ray(point,-direction);
	vector<Shape*>::const_iterator prim=objects.begin();
	for(;prim!=objects.end(); ++prim){
		if ((*prim)->intersect(ray) >= EPSILON){
			return false;
		}
	}
	return true;
}

/***  Point Light  ***/
PointLight::PointLight(const vec3& colour,const vec3& poi, double con, double lin, double quad) {
	color = colour;
	point = poi;
	constant = con;
	linear = lin;
	quadratic = quad;
}

vec3 PointLight::shade(const Intersection& hit, const vector<Shape*>& objects, const vec3& normal){
	if( !isVisible(hit.point,objects) ){
		return vec3(0.0,0.0,0.0);
	}
	vec3 direction = glm::normalize(point-hit.point);
	vec3 shade = max(0.0,glm::dot(normal,direction)) * hit.primative->diffuse;
	
	vec3 half = glm::normalize(hit.sourceDirection+direction);
	double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
	shade += phong * hit.primative->specular;
	shade *= color;
	double dist = glm::distance(point,hit.point);
	shade /= constant + linear*dist + quadratic*dist*dist;
	return shade;
}

bool PointLight::isVisible(const vec3& p, const vector<Shape*>& objects) {
	vec3 direction = glm::normalize(point-p);
	Ray ray(p,direction);
	vector<Shape*>::const_iterator prim=objects.begin();
	for(;prim!=objects.end(); ++prim){
		if ((*prim)->intersect(ray) >= EPSILON) return false;
	}
	return true;
}

