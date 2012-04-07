/* Class definition for a shape */
#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include "Transform.h"
#include "Ray.h"

class Shape {
	public:
		virtual double intersect(Ray&)=0;
		virtual vec3 getNormal(vec3&)=0;
		
		/* material properties */
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		double shininess;
		vec3 emission;
};

#include "Triangle.h"
#include "Sphere.h"

#endif 