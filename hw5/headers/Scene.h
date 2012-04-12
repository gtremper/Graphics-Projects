#ifndef SCENE_H
#define SCENE_H

class Scene {
	public:
		vec3 w;
		vec3 u;
		vec3 v;
		int width;
		int height;
		int maxdepth;
		std::string filename;
		double fovy;
		vec3 eye;
		std::vector<Shape*> objects;
		
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		double shininess;
		vec3 emission;
		
		double constant;
		double linear;
		double quadratic;
		
		Scene();
		~Scene();
		void castEyeRay(int,int,Ray&);
		void setCoordinateFrame(vec3&,vec3&);
		void parseLine(std::string,std::stack<mat4>&,
			std::vector<vec3>&,std::vector<vec3>&,std::vector<vec3>&);
		void parse(char*);
};


#endif