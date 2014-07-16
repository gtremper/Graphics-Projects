// Transform header file to define the interface. 
// The class is all static for simplicity
// You need to implement left, up and lookAt
// Rotate is a helper function

// Include the helper glm library, including matrix transform extensions

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// glm provides vector, matrix classes like glsl
// Typedefs to make code more readable 

typedef glm::dmat2 mat2 ;
typedef glm::dmat3 mat3 ;
typedef glm::dmat4 mat4 ; 
typedef glm::dvec2 vec2 ;
typedef glm::dvec3 vec3 ; 
typedef glm::dvec4 vec4 ; 

const double pi = 3.14159265358979 ; // For portability across platforms

class Transform	 
{
public:
	Transform();
	virtual ~Transform();
	static mat4 rotate(double degrees, const vec3& axis) ;
	static mat4 scale(double sx, double sy, double sz) ; 
	static mat4 translate(double tx, double ty, double tz);
};

#endif

