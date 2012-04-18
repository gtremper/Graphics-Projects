/* Class definitions for Triangles and Spheres */
#include <iostream>
#include <vector>

#include "Shapes.h"
#include "Intersection.h"

#define EPSILON 0.0000005

using namespace std;

/***  RAY  ***/
Ray::Ray(const vec3& o,const vec3& d){
	origin = o;
	direction = d;
}

vec3 Ray::getPoint(double t) {
	return origin + t*direction;
}

/***  INTERSECTION  ***/
Intersection::Intersection(vector<Shape*>& objects, Ray& ray) {
	double min_t = DBL_MAX;
	primative = NULL;
	for(vector<Shape*>::iterator prim=objects.begin(); prim!=objects.end(); prim++) {
		double t = (*prim)->intersect(ray);
		if (t>0 && t<min_t){
			primative = *prim;
			min_t = t;
		}
	}
	point = ray.getPoint(min_t);
	sourceDirection = -ray.direction;
}

/***  TRIANGLE  ***/
Triangle::Triangle(vec3 point0, vec3 point1, vec3 point2) {
	p0 = point0;
	p1 = point1;
	p2 = point2;
	n0 = glm::normalize(glm::cross(p1-p0,p2-p0));
}


double Triangle::intersect(Ray& ray){
	vec3 col1 = p1-p0;
	vec3 col2 = p2-p0;
	mat3 M = mat3(col1, col2, -ray.direction);
	double det = glm::determinant(M);
	if (det<EPSILON && det>-EPSILON) return -1.0;
	
	M[0] = ray.origin-p0;
	double alpha = glm::determinant(M)/det;
	if (alpha<0 || alpha>1) return -1.0;
	M[0] = col1;
	M[1] = ray.origin-p0;

	double beta = glm::determinant(M)/det;
	if (beta<0 || beta+alpha>1) return -1.0;
	M[1] = col2;
	M[2] = ray.origin-p0;
	return glm::determinant(M)/det;
}

vec3 Triangle::getNormal(vec3& hit){
	return n0;
}

/***  NORMTRIANGLE  ***/
NormTriangle::NormTriangle(vec3 point0, vec3 point1, vec3 point2,
							vec3 norm0, vec3 norm1, vec3 norm2) {
	p0 = point0;
	p1 = point1;
	p2 = point2;
	n0 = glm::normalize(norm0);
	n1 = glm::normalize(norm1);
	n2 = glm::normalize(norm2);
}

vec3 NormTriangle::getNormal(vec3& hit){
	mat2 M = mat2(p1[0]-p0[0], p1[1]-p0[1], p2[0]-p0[0], p2[1]-p0[1]);
	double det = glm::determinant(M);
	M[0][0] = hit[0]-p0[0];
	M[0][1] = hit[1]-p0[1];
	double beta = glm::determinant(M)/det;
	M[0][0] = p1[0]-p0[0];
	M[0][1] = p1[1]-p0[1];
	M[1][0] = hit[0]-p0[0];
	M[1][1] = hit[1]-p0[1];
	double gamma = glm::determinant(M)/det;
	return glm::normalize((1-beta-gamma)*n0 + beta*n1 + gamma*n2);
}

/***  SPHERE  ***/
double Sphere::intersect(Ray& ray) {
	vec3 direction = glm::normalize(vec3(inv * vec4(ray.direction,0)));
	vec3 origin =vec3(inv * vec4(ray.origin,1));
	
	double b = 2.0 * glm::dot(direction, origin);
	double c = glm::dot(origin,origin) - 1.0;
	double det = b*b - 4.0*c;
	if (det<0.0) return -1.0;
	det = sqrt(det);
	
	double t1 = (-b+det)/2.0;
	double t2 = (-b-det)/2.0;
	
	if (t1<0.0 && t2<0.0) return -1.0;
	
	if (t2>0.0) {
		vec4 hit = mv * vec4(origin+t2*direction,1);
		return glm::distance(ray.origin,vec3(hit));
	} else { //t1 is closer
		vec4 hit = mv * vec4(origin+t1*direction,1);
		return glm::distance(ray.origin,vec3(hit));
	}
}

vec3 Sphere::getNormal(vec3& hit){
	return glm::normalize(vec3(glm::transpose(inv)*inv*vec4(hit,1.0)));
}
