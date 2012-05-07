/* Light.cpp: Defines methods for the light class */
#include <vector>
#include <math.h>
#include <iostream>

#include "Shapes.h"
#include "Intersection.h"
#include "Light.h"

#define EPSILON 0.0001

using namespace std;


/***  Directional Light  ***/
DirectionalLight::DirectionalLight(const vec3& c,const vec3& dir) {
	color = c;
	direction = glm::normalize(dir);
}

vec3 DirectionalLight::shade(const Intersection& hit, const vector<Shape*>& objects, const vec3& normal){
	if(!isVisible(hit.point+EPSILON*normal,objects)) {
		return vec3(0.0,0.0,0.0);
	}
	vec3 shade = max(0.0,glm::dot(normal,direction)) * hit.primative->diffuse;
	vec3 half = glm::normalize(hit.sourceDirection+direction);
	double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
	shade += phong * hit.primative->specular;
	shade *= color;
	
	return shade;
}

bool DirectionalLight::isVisible(const vec3& point, const vector<Shape*>& objects) {
	Ray ray(point,direction);
	vector<Shape*>::const_iterator prim=objects.begin();
	for(;prim!=objects.end(); ++prim){
		if ((*prim)->intersect(ray) >= 0.0){
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

vec3 PointLight::shade(const Intersection& hit, const vector<Shape*>& objects, const vec3& normal) {
	double totalshots = 20;
	vec3 totalShade(0,0,0);
	for(int i =0; i<totalshots; i++){
		if(!isVisible(hit.point+EPSILON*normal,objects)) {
			continue;
		}
		vec3 direction = glm::normalize(point-hit.point);
		vec3 shade = max(0.0,glm::dot(normal,direction)) * hit.primative->diffuse;
	
		vec3 half = glm::normalize(hit.sourceDirection+direction);
		double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
		shade += phong * hit.primative->specular;
		shade *= color;
		double dist = glm::distance(point,hit.point);
		shade *= 1.0/(constant + linear*dist + quadratic*dist*dist);
		totalShade += shade;
	}
	return totalShade/totalshots;
}

bool PointLight::isVisible(const vec3& p, const vector<Shape*>& objects) {
	vec3 randPoint;
	double radius = 0.15;
	double randomNum1 = ((double)rand()/(double)RAND_MAX);
	double randomNum2 = ((double)rand()/(double)RAND_MAX);
	double q = 2. * M_PI * randomNum1;
	double f = acos(2. * randomNum2 - 1);
	randPoint[0] = radius*cos(q)*sin(f);
	randPoint[1] = radius*sin(q)*sin(f);
	randPoint[2] = radius*cos(f);
	
	vec3 direction = glm::normalize(randPoint-p);
	Ray ray(p,direction);
	vector<Shape*>::const_iterator prim=objects.begin();
	double dist = glm::distance(randPoint,p);
	for(;prim!=objects.end(); ++prim){
		double t = (*prim)->intersect(ray);
		if (t >= 0.0 && t<dist ) return false;
	}
	return true;
}

