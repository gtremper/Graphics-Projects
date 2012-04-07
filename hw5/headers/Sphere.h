/* Class definition for a sphere */

#ifndef SPHERE_H
#define SPHERE_H

class Sphere : public Shape {
	public:
		double intersect(Ray&);
		vec3 getNormal(vec3&);
	
		mat4 mv;
		mat4 inv;	
};

#endif