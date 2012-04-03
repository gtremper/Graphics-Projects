/* Class definition for a sphere */

#ifndef SPHERE_H
#define SPHERE_H

class Sphere : public Shape {
	public:
		Sphere(vec3,double);
		virtual ~Sphere();
		virtual double intersect(Ray&);
		virtual vec3 getNormal();
		
		vec3 center;
		double radius;	
};

#endif