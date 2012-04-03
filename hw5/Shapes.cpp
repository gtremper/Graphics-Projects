/* Class definitions for Triangles and Spheres */
#include "Shapes.h"
#include "Intersection.h"
#include <iostream>
#define EPSILON 0.00000000005


using namespace std;

/***  RAY  ***/
vec3 Ray::getPoint(double t) {
	return origin + t*direction;
}

Intersection::Intersection(vector<Shape*>& objects, Ray& ray) {
	double min_t = DBL_MAX;
	primative = NULL;
	for(vector<Shape*>::iterator prim=objects.begin(); prim!=objects.end(); prim++) {
		double t = (*prim)->intersect(ray);
		if (t>0 && t<min_t){
			primative = *prim;
			min_t = t;
			//cout << "Intersected!" << endl;
		}
	}
	point = ray.getPoint(min_t);
}

Intersection::~Intersection(){}
Shape::Shape(){}
Shape::~Shape(){}
double Shape::intersect(Ray& ray) {
	cout << "This should never be called \"intersect\""<<endl;
	return 0;
}
vec3 Shape::getNormal(){
	cout << "This should never be called \"getNormal\""<<endl;
	return vec3(0,0,0);
}


/***  TRIANGLE  ***/
Triangle::Triangle(vec3 point0, vec3 point1, vec3 point2) {
	p0 = point0;
	p1 = point1;
	p2 = point2;
	faceNormal = glm::normalize(glm::cross(p1-p0,p2-p0));
}


double Triangle::intersect(Ray& ray){
	vec3 col1 = p1-p0;
	vec3 col2 = p2-p0;
	mat3 M = mat3(col1, col2, -ray.direction);
	double det = glm::determinant(M);
	
	//cout << "INTERSECT1" << endl;
	//if (det<EPSILON && det>-EPSILON) return -1.0;
	
	M[0] = ray.origin-p0;
	float alpha = glm::determinant(M)/det;
	//cout << "INTERSECT2" << endl;
	if (alpha<0 || alpha>1) return -1.0;
	M[0] = col1;
	M[1] = ray.origin-p0;
	//cout << "INTERSECT3" << endl;
	float beta = glm::determinant(M)/det;
	if (beta<0 || beta>1) return -1.0;
	M[1] = col2;
	M[2] = ray.origin-p0;
	return glm::determinant(M)/det;
}

vec3 Triangle::getNormal(){
	return faceNormal;
}

Triangle::~Triangle(){}

/* SPHERE */
Sphere::Sphere(vec3 cen, double rad) {
	center = cen;
	radius = rad;
}


double Sphere::intersect(Ray& ray) {
	
	//Need to at acess to "eye" and "center" from scene.....
	
	/*
	double a = glm::dot(ray.direction, ray.direction);
	double b = 2.0 * glm::dot(ray.direction,eye-center);
	double c = glm::dot(eye-center,eye-center) - radius*radius;
	
	double det = b*b - 4.0*a*c;
	if (det<0.0) return -1.0;
	
	double t1 = (-b+det)/(2*a);
	double t2 = (-b-det)/(2*a);
	
	if (t1>0.0 && t2>0.0) return min(t1,t2);
	
	if (t1>0.0) return t1;
	if (t2>0.0) return t0;
	*/
	
	return -1.0;
}

vec3 Sphere::getNormal(){
	//not implemented yet
	return vec3(0,0,0);
}

Sphere::~Sphere(){}


