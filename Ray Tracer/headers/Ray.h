/* Class description for Ray */
#ifndef RAY_H
#define RAY_H

class Ray {
public:
	Ray(){};
	Ray(const vec3&,const vec3&);
	
	vec3 getPoint(double);
	vec3 origin;
	vec3 direction;	
};

#endif