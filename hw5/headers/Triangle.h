/* Class definition for triangle */

#ifndef TRIANGLE_H
#define TRIANGLE_H

class Triangle : public Shape {
	public:
		Triangle(vec3,vec3,vec3);
		double intersect(Ray&);
		virtual vec3 getNormal();
		
		vec3 p0;
		vec3 p1;
		vec3 p2;
		vec3 faceNormal;
};

class NormTriangle : private Triangle {
	
};

#endif