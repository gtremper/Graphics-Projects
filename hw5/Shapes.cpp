/* Class definitions for Triangles and Spheres */
#include <iostream>
#include <vector>

#include "Shapes.h"
#include "Intersection.h"

#define EPSILON 0.00000001

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
	double minx = min(min(p0[0],p1[0]),p2[0]);
	double maxx = max(max(p0[0],p1[0]),p2[0]);
	double miny = min(min(p0[1],p1[1]),p2[1]);
	double maxy = max(max(p0[1],p1[1]),p2[1]);
	double minz = min(min(p0[2],p1[2]),p2[2]);
	double maxz = max(max(p0[2],p1[2]),p2[2]);
	aabb = AABB(minx,maxx,miny,maxy,minz,maxz);
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

/***  AABB  ***/
AABB::AABB(double minx, double maxx, double miny, double maxy, double minz, double maxz) {
	bounds[0] = minx;
	bounds[1] = maxx;
	bounds[2] = miny;
	bounds[3] = maxy;
	bounds[4] = minz;
	bounds[5] = maxz;
	center[0] = (minx + maxx)/2.0;
	center[1] = (miny + maxy)/2.0;
	center[2] = (minz + maxz)/2.0;
}

bool intersect1D(double start, double dir, double axisMin, double axisMax, double& enter, double& ex){
	// Parallel
	if(dir<EPSILON && dir>-EPSILON){
		return (start>=axisMin && start<=axisMax);
	}
	
	//intersection parameters
	double t0 = (axisMin-start)/dir;
	double t1 = (axisMax-start)/dir;
	
	if(t0>t1){
		double temp = t0;
		t1 = t0;
		t0 = temp;
	}
	
	if(t0>ex || t1<enter) return false;
	
	t0 = max(t0,enter);
	t1 = min(t1,ex);
	
	return true;
}

bool AABB::intersect(Ray& ray){
	double ex = 999999999999;
	double enter = -999999999;
	
	if (!intersect1D(ray.origin[0],ray.direction[0],bounds[0],bounds[1],enter,ex)) return false;
	if (!intersect1D(ray.origin[1],ray.direction[1],bounds[2],bounds[3],enter,ex)) return false;
	if (!intersect1D(ray.origin[2],ray.direction[2],bounds[4],bounds[5],enter,ex)) return false;
	
	return true;
}






