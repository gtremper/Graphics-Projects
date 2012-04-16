/* Class definition for Intersection */
#ifndef INTERSECTION_H
#define INTERSECTION_H

class Intersection {
public:
	Intersection(std::vector<Shape*>&, Ray&);
	Shape* primative;
	vec3 point;
	vec3 sourceDirection;
};

#endif