/* Class definition for a shape */
#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include "Transform.h"
#include "Ray.h"

class Shape {
	public:
		Shape();
		virtual ~Shape();
		virtual double intersect(Ray& ray);
		virtual vec3 getNormal();
};

#include "Triangle.h"
#include "Sphere.h"

#endif 