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
		int maxverts;
		int maxvertnorms;
		std::string filename;
		double fov;
		vec3 eye;
		std::vector<Shape*> objects;
		
		Scene();
		void castEyeRay(int,int,Ray&);
		void setCoordinateFrame(vec3&,vec3&);
		void parseLine(std::string,std::stack<mat4>&,
			std::vector<vec3>&,std::vector<vec3>&,std::vector<vec3>&);
		void parse(char*);
};


#endif