/* Deals with the rendered scene */
#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <string>
#include <vector>
#include "float.h"

#include "FreeImage.h"
#include "Shapes.h"
#include "Intersection.h"
#include "Light.h"
#include "Scene.h"

using namespace std;


/***  SCENE  ***/

// sets default values
Scene::Scene(char* file) {
	filename = "OUTPUT.png";
	maxdepth = 5; 
	ambient = vec3(0.2,0.2,0.2);
	diffuse = vec3(0,0,0);
	specular = vec3(0,0,0);
	shininess = 0;
	emission = vec3(0,0,0);
	constant = 1;
	linear = 0;
	quadratic = 0;
	parse(file);
}

Scene::~Scene() {
	vector<Shape*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++){
		delete *it;
	}
	vector<Light*>::iterator l;
	for(l=lights.begin(); l!=lights.end(); l++){
		delete *l;
	}
}


/************************************************* 
** The shot ray is stored in the argument "ray" **
**	A little jenky but whatever, its faster.    **
*************************************************/ 
 
void Scene::castEyeRay(int i, int j, Ray& ray){
	double half = width/2.0;
	double alpha = tan(fovy*pi*width/(height*360.0)) * ((i-half)/half);
	half = height/2.0;
	double beta = tan(fovy*pi/360.0) * ((j-half)/half);
	ray.origin = eye;
	ray.direction = glm::normalize(alpha*u + beta*v - w);
}

/***********************************************
** Parsing input and setting global variables **
***********************************************/

void Scene::setCoordinateFrame(vec3& lookat, vec3& up){
	w = glm::normalize(eye - lookat);
	u = glm::normalize(glm::cross(up, w));
	v = glm::cross(w,u);
}

void Scene::parseLine(string l, stack<mat4>& mv, vector<vec3>& verts, 
				vector<vec3>& normverts, vector<vec3>& norms){
					
	stringstream line(l);
	string cmd;
	line >> cmd;
	if (cmd[0]=='#' || cmd=="") { //comment or blank line
		return;
	} else if (cmd == "size") {
		line >> width >> height;
	} else if (cmd == "maxdepth") {
		line >> maxdepth;
	} else if (cmd == "output") {
		line >> filename;
	} else if (cmd == "camera") {
		double arg1, arg2, arg3;
		line >> arg1 >> arg2 >> arg3;
		eye = vec3(arg1,arg2,arg3);
		line >> arg1 >> arg2 >> arg3;
		vec3 lookat = vec3(arg1,arg2,arg3);
		line >> arg1 >> arg2 >> arg3;
		vec3 up = vec3(arg1,arg2,arg3);
		setCoordinateFrame(lookat,up);
		line >> fovy;
	} else if (cmd == "sphere") {
		double arg1, arg2, arg3, arg4;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		line >> arg4;
		mat4 trans = mv.top();
		trans *= Transform::translate(arg1,arg2,arg3);
		trans *= Transform::scale(arg4,arg4,arg4);
		Sphere* s = new Sphere();
		s->mv = trans;
		s->inv = glm::inverse(trans);
		s->ambient = ambient;
		s->diffuse = diffuse;
		s->specular = specular;
		s->shininess = shininess;
		s->emission = emission;
		objects.push_back(s);
	} else if (cmd == "maxverts") {
		int maxverts;
		line >> maxverts;
		verts.reserve(maxverts);
	} else if (cmd == "maxvertnorms") {
		int maxvertnorms;
		line >> maxvertnorms;
		verts.reserve(maxvertnorms);
		verts.reserve(maxvertnorms);
	} else if (cmd == "vertex") {
		double arg1, arg2, arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		vec3 v(arg1,arg2,arg3);
		verts.push_back(v);
	} else if (cmd == "vertexnormal") {
		double arg1, arg2, arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		vec3 v(arg1,arg2,arg3);
		normverts.push_back(v);
		line >> arg1;
		line >> arg2;
		line >> arg3;
		vec3 n(arg1,arg2,arg3);
		norms.push_back(n);
	} else if (cmd == "tri") {
		int a1, a2, a3;
		line >> a1 >> a2 >> a3;
		mat4 top =  mv.top();
		vec3 v1 = vec3(top * vec4(verts[a1],1));
		vec3 v2 = vec3(top * vec4(verts[a2],1));
		vec3 v3 = vec3(top * vec4(verts[a3],1));
		Triangle* t = new Triangle(v1,v2,v3);
		t->ambient = ambient;
		t->diffuse = diffuse;
		t->specular = specular;
		t->shininess = shininess;
		t->emission = emission;
		objects.push_back(t);
	} else if(cmd == "trinormal") {
		int a1,a2,a3;
		line >> a1 >> a2 >> a3;
		//NormTriangle* t = new NormTriangle(normverts[a1],normverts[a2],normverts[a3],
		//								norms[a1],norms[a2],norms[a3]);
		//objects.push_back(t);
	} else if(cmd == "translate") {
		double arg1,arg2,arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		mv.top() *= Transform::translate(arg1, arg2, arg3);
	} else if(cmd == "rotate") {
		double arg1,arg2,arg3,arg4;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		line >> arg4;
		mv.top() *= Transform::rotate(arg4,vec3(arg1,arg2,arg3));
	} else if (cmd=="scale") {
		double arg1,arg2,arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		mv.top() *= Transform::scale(arg1, arg2, arg3);
	} else if (cmd == "pushTransform") {
		mv.push(mv.top());
	} else if (cmd == "popTransform"){
		mv.pop();
	} else if (cmd == "directional") {
		double x,y,z,r,g,b;
		line >> x >> y >> z >> r >> g >> b;
		DirectionalLight* light = new DirectionalLight(vec3(r,g,b),vec3(x,y,z));
		lights.push_back(light);
	} else if (cmd == "point") {
		double x,y,z,r,g,b;
		line >> x >> y >> z >> r >> g >> b;
		PointLight* light = new PointLight(vec3(r,b,g),vec3(x,y,z),constant,linear,quadratic);
		lights.push_back(light);
	} else if (cmd == "attenuation") {
		line >> constant >> linear >> quadratic;
	} else if (cmd == "ambient") {
		double arg1, arg2, arg3;
		line >> arg1 >> arg2 >> arg3;
		ambient = vec3(arg1,arg2,arg3);
	} else if (cmd == "diffuse") {
		double arg1, arg2, arg3;
		line>>arg1;
		line>>arg2;
		line>>arg3;
		diffuse = vec3(arg1,arg2,arg3);
	} else if (cmd == "specular") {
		double arg1, arg2, arg3;
		line>>arg1;
		line>>arg2;
		line>>arg3;
		specular = vec3(arg1,arg2,arg3);
	} else if (cmd == "shininess") {
		line>>shininess;
	} else if (cmd == "emission") {
		double arg1, arg2, arg3;
		line>>arg1;
		line>>arg2;
		line>>arg3;
		emission = vec3(arg1,arg2,arg3);
	}
	cout << cmd << endl;
}

void Scene::parse(char* filename) {
	ifstream  file(filename, ifstream::in);
	string line;
	if (file.is_open()) {
		stack<mat4> modelview;
		modelview.push(mat4(1.0));
		vector<vec3> verts;
		vector<vec3> normverts;
		vector<vec3> norms;
		while (file.good()) {
			getline(file, line);
			parseLine(line, modelview, verts, normverts, norms);
		}
	} else {
		cerr << "Unable to open file " << filename << endl;
		exit(1);
	}
	file.close();
}