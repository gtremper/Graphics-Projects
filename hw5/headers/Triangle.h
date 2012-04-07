/* Class definition for triangle */

#ifndef TRIANGLE_H
#define TRIANGLE_H

class Triangle : public Shape {
	public:
		Triangle(){};
		Triangle(vec3,vec3,vec3);
		double intersect(Ray&);
		virtual vec3 getNormal(vec3&);
	
		vec3 p0;
		vec3 p1;
		vec3 p2;
		vec3 n0;
};


class NormTriangle : private Triangle {
public:
	NormTriangle(vec3,vec3,vec3,vec3,vec3,vec3);
	vec3 getNormal(vec3&);

	vec3 n1;
	vec3 n2;	
};

#endif