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
	Ray ray(point,direction);
	Intersection hit = tree.intersect(ray);
	return !hit.primative;
}

/***  Point Light  ***/
PointLight::PointLight(const vec3& colour,const vec3& poi, double con, double lin, double quad) {
	color = colour;
	point = poi;
	constant = con;
	linear = lin;
	quadratic = quad;
}

vec3 PointLight::shade(const Intersection& hit, TreeNode& tree, const vec3& normal) {
	double totalshots = 1;
	int numberOfHits = 0;
	for(int i =0; i<totalshots; i++){
		if( isVisible(hit.point+EPSILON*normal,tree) ) {
			numberOfHits++;
		}
	}
	if (numberOfHits == 0) return vec3(0,0,0);

	vec3 direction = glm::normalize(point-hit.point);
	vec3 shade = max(0.0,glm::dot(normal,direction)) * hit.primative->diffuse;

	vec3 half = glm::normalize(hit.sourceDirection+direction);
	double phong = pow( max(0.0,glm::dot(half,normal)) , hit.primative->shininess);
	shade += phong * hit.primative->specular;
	shade *= color;
	double dist = glm::distance(point,hit.point);
	shade *= 1.0/(constant + linear*dist + quadratic*dist*dist);
	
	return (numberOfHits/totalshots)*shade;
}

bool PointLight::isVisible(const vec3& p, TreeNode& tree) {
	vec3 randPoint;
	double radius = 0.0;
	double randomNum1 = ((double)rand()/(double)RAND_MAX);
	double randomNum2 = ((double)rand()/(double)RAND_MAX);
	double q = 2. * M_PI * randomNum1;
	double f = acos(2. * randomNum2 - 1);
	randPoint[0] = radius*cos(q)*sin(f);
	randPoint[1] = radius*sin(q)*sin(f);
	randPoint[2] = radius*cos(f);
	
	randPoint+=point;
	
	vec3 direction = glm::normalize(randPoint-p);
	Ray ray(p,direction);
	double dist = glm::distance(randPoint,p);
	Intersection hit = tree.intersect(ray);
	if(hit.primative){
		if(glm::distance(p,hit.point) < dist){
			return false;
		}
	}
	return true;
}

