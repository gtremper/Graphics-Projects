/* Class definition for lights */
#ifndef LIGHT_H
#define LIGHT_H

class Light {
public:
	virtual vec3 shade(const Intersection& ,const std::vector<Shape*>& ,const vec3&)=0;	
	vec3 color;
};

class DirectionalLight : public Light {
public:
	DirectionalLight(const vec3& color,const vec3& dir);
	vec3 shade(const Intersection& hit, const std::vector<Shape*>& objects,const vec3&);	
	bool isVisible(const vec3& point, const std::vector<Shape*>& objects);
	vec3 direction;
};

class PointLight : public Light {
	PointLight(const vec3& color,const vec3& p, double,double,double);
	vec3 shade(const Intersection& hit, const std::vector<Shape*>& objects,const vec3&);	
	bool isVisible(const vec3& point, const std::vector<Shape*>& objects);
	vec3 point;
	double constant;
	double linear;
	double quadratic;
};

#endif