// Transform.cpp: implementation of the Transform class.

#include "Transform.h"
#include <iostream>

//Takes as input the current eye position, and the current up vector.
//up is always normalized to a length of 1.
//eye has a length indicating the distance from the viewer to the origin

// Helper rotation function.  Please implement this.  

mat3 Transform::rotate(const float degrees, const vec3& axis) {
	mat3 parallel( cos(degrees*pi/180.0) );
	double x = axis[0];
	double y = axis[1];
	double z = axis[2];
	mat3 rotation(x*x, x*y, x*z, x*y, y*y, y*z, x*z, y*z, z*z);
	rotation = (1-cos(degrees*pi/180.0))*rotation;
	mat3 cross(0.0, -z, y, z, 0.0, -x, -y, x, 0);
	cross = sin(degrees*pi/180.0)*cross;
	mat3 result = parallel + rotation + cross;
	return result; 
}

void Transform::left(float degrees, vec3& eye, vec3& up) {
	mat3 R = rotate(degrees,up);
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
	vec3 a = eye-center;
	vec3 w = glm::normalize(a);
	vec3 u = glm::cross(up,a);
	u = glm::normalize(u);
	vec3 v = glm::cross(w,u);
	double utrans = -glm::dot(a,u);
	double vtrans = -glm::dot(a,v);
	double wtrans = -glm::dot(a,w);
	mat4 M(u[0], u[1], u[2], utrans, v[0], v[1], v[2], vtrans,
			w[0], w[1], w[2], wtrans, 0, 0, 0, 1);
	return M; 
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar) {
	//Implement me!!!	
	mat4 implementMe;
	return implementMe;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
	//Implement me!!!	
	mat4 implementMe;
	return implementMe;	
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
	//Implement me!!!
	mat4 implementMe;
	return implementMe;	
}

/* Trash */
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
