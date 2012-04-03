/* Class definition for a shape */
#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include "Transform.h"
#include "Ray.h"

class Shape {
	public:
		virtual ~Shape(){};
		virtual double intersect(Ray& ray)=0;
		virtual vec3 getNormal()=0;
		
		/* material properties */
		vec3 ambient;
};

#include "Triangle.h"
#include "Sphere.h"

#endif 