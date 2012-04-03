/* Class description for Ray */
#ifndef RAY_H
#define RAY_H

class Ray {
public:
	vec3 getPoint(double);
	vec3 origin;
	vec3 direction;	
};

#endif