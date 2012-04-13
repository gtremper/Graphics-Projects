/* Class description for Ray */
#ifndef RAY_H
#define RAY_H

class Ray {
public:
	Ray(){};
	Ray(vec3 o,vec3 d);
	
	vec3 getPoint(double);
	vec3 origin;
	vec3 direction;	
};

#endif