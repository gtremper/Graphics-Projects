/* Transform.cpp: implementation of the Transform class. */

#include "Transform.h"
#include <iostream>

mat4 Transform::rotate(double degrees, const vec3& axis) {
	mat3 parallel( cos(degrees*pi/180.0) );
	double x = axis[0];
	double y = axis[1];
	double z = axis[2];
	mat3 rotation(x*x, x*y, x*z, x*y, y*y, y*z, x*z, y*z, z*z);
	rotation = (1-cos(degrees*pi/180.0))*rotation;
	mat3 cross(0.0, -z, y, z, 0.0, -x, -y, x, 0.0);
	cross = sin(degrees*pi/180.0)*cross;
	mat3 result = parallel + rotation + cross;
	return mat4(glm::transpose(result));
}

mat4 Transform::scale(double sx, double sy, double sz) {
	mat4 S(sx,0,0,0,0,sy,0,0,0,0,sz,0,0,0,0,1);
	return S;	
}

mat4 Transform::translate(double tx, double ty, double tz) {
	mat4 T(1,0,0,tx,0,1,0,ty,0,0,1,tz,0,0,0,1);
	return glm::transpose(T);	
}

/* Unused constructor and deconstructer */
Transform::Transform(){}
Transform::~Transform(){}

// Some notes about using glm functions.
// You are ONLY permitted to use glm::dot glm::cross glm::normalize
// Do not use more advanced glm functions (in particular, directly using 
// glm::lookAt is of course prohibited).  
 
