/* Class definition for Intersection */
#ifndef INTERSECTION_H
#define INTERSECTION_H

class Intersection {
public:
	Intersection(std::vector<Shape*>&, Ray&);
	virtual ~Intersection();
	Shape* primative;
	vec3 point;
};

#endif