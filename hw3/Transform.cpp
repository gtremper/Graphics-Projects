// Transform.cpp: implementation of the Transform class.

#include "Transform.h"
#include <iostream>


//Takes as input the current eye position, and the current up vector.
//up is always normalized to a length of 1.
//eye has a length indicating the distance from the viewer to the origin

mat3 Transform::rotate(const float degrees, const vec3& axis) {
	mat3 parallel( cos(degrees*pi/180.0) );
	float x = axis[0];
	float y = axis[1];
	float z = axis[2];
	mat3 rotation(x*x, x*y, x*z, x*y, y*y, y*z, x*z, y*z, z*z);
	rotation = (1-cos(degrees*pi/180.0))*rotation;
	mat3 cross(0.0, -z, y, z, 0.0, -x, -y, x, 0.0);
	cross = sin(degrees*pi/180.0)*cross;
	mat3 result = parallel + rotation + cross;
	return result; 
}

vec3 Transform::direction(float &yaw, float &pitch, const vec3& dir,const vec3& up, bool fly) {
	mat3 M = rotate(yaw,up);
	vec3 final = dir * M;
	if (fly){
		vec3 cross = glm::cross(final,up);
		M = rotate(pitch,cross);
		final = final * M;
	}
	return final;
}

void Transform::left(float degrees, vec3& eye, vec3& up) {
	
	mat3 R = rotate(degrees,glm::normalize(up));
	eye = eye * R;
}


void Transform::up(float degrees, vec3& eye, vec3& up) {
	vec3 side = glm::cross(up,eye);
	side = glm::normalize(side);
	mat3 R = rotate(-degrees,side);
	eye = eye*R;
	up = up*R;
}


mat4 Transform::lookAt(const vec3& eye, const vec3& center, const vec3& up) {
	vec3 a = eye-center; //vector from center to eye
	vec3 w = glm::normalize(a);
	vec3 u = glm::cross(up,w);
	u = glm::normalize(u);
	vec3 v = glm::cross(w,u);
	float utrans = -glm::dot(eye,u);
	float vtrans = -glm::dot(eye,v);
	float wtrans = -glm::dot(eye,w);
	mat4 M(u[0], u[1], u[2], utrans, v[0], v[1], v[2], vtrans,
			w[0], w[1], w[2], wtrans, 0, 0, 0, 1);
	return M;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar) {
	float d = 1/tan(pi*fovy/360);
	float a = -(zFar+zNear)/(zFar-zNear);
	float b = -(2*zFar*zNear)/(zFar-zNear);
	mat4 P(d/aspect,0,0,0,0,d,0,0,0,0,a,b,0,0,-1,0);
	return P;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
	mat4 S(sx,0,0,0,0,sy,0,0,0,0,sz,0,0,0,0,1);
	return S;	
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
	mat4 T(1,0,0,tx,0,1,0,ty,0,0,1,tz,0,0,0,1);
	return T;	
}

/* Unused constructor and deconstructer */
Transform::Transform(){}
Transform::~Transform(){}

// Some notes about using glm functions.
// You are ONLY permitted to use glm::dot glm::cross glm::normalize
// Do not use more advanced glm functions (in particular, directly using 
// glm::lookAt is of course prohibited).  

// You may use overloaded operators for matrix-vector multiplication 
// But BEWARE confusion between opengl (column major) and row major 
// conventions, as well as what glm implements. 
// In particular, vecnew = matrix * vecold may not implement what you think 
// it does.  It treats matrix as column-major, in essence using the transpose.
// We recommend using row-major and vecnew = vecold * matrix 
// Preferrably avoid matrix-matrix multiplication altogether for this hw.  
