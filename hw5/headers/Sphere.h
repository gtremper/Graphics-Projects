/* Class definition for a sphere */

#ifndef SPHERE_H
#define SPHERE_H

class Sphere : public Shape {
	public:
		Sphere(vec3,double);
		double intersect(Ray&);
		vec3 getNormal();
	private:	
		vec3 center;
		double radius;	
};

#endif