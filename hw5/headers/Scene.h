#ifndef SCENE_H  
#define SCENE_H

#include "KDTree.h"

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
		double fovx;
		vec3 eye;
		std::vector<Shape*> objects;
		AABB sceneAABB;
		TreeNode* KDTree;
		
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		double shininess;
		vec3 emission;
		
		std::vector<Light*> lights;
		double constant;
		double linear;
		double quadratic;
		
		Scene(char*);
		~Scene();
		Ray castEyeRay(int,int);
		void setCoordinateFrame(vec3&,vec3&);
		void parseLine(std::string,std::stack<mat4>&,
			std::vector<vec3>&,std::vector<vec3>&,std::vector<vec3>&);
		void parse(char*);
		void updateAABB(vec3&);
};


#endif