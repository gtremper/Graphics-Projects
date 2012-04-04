/* Deals with the rendered scene */
#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <string>
#include "float.h"

#include "FreeImage.h"
#include "Shapes.h"
#include "Scene.h"

using namespace std;


// sets default values
Scene::Scene() {
	filename = "OUTPUT";
	maxdepth = 5; 
	ambient = vec3(0.2,0.2,0.2);
	diffuse = vec3(0,0,0);
	specular = vec3(0,0,0);
	shininess = 0;
	emission = vec3(0,0,0);
}

Scene::~Scene() {
	vector<Shape*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++){
		delete *it;
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
	//cout << "EyeRay"<<j<<"x"<<i<<": "<<
	//	ray.direction[0]<<" "<<ray.direction[1]<<" "<<ray.direction[2]<<endl;
}

/***********************************************
** Parsing input and setting global variables **
***********************************************/

void Scene::setCoordinateFrame(vec3& lookat, vec3& up){
	w = glm::normalize(eye - lookat);
	//cout << "w:("<<w[0]<<", "<<w[1]<<", "<<w[2]<<")"<<endl;
	u = glm::normalize(glm::cross(up, w));
	//cout << "u:("<<u[0]<<", "<<u[1]<<", "<<u[2]<<")"<<endl;
	v = glm::cross(w,u);
	//cout << "v:("<<v[0]<<", "<<v[1]<<", "<<v[2]<<")"<<endl;
}

void Scene::parseLine(string l, stack<mat4>& mv, vector<vec3>& verts, 
				vector<vec3>& normverts, vector<vec3>& norms){
					
	stringstream line(l);
	string cmd;
	line >> cmd;
	if (cmd[0]=='#' || cmd=="") { //comment or blank line
		return;
	} else if (cmd == "size") {
		line >> width;
		line >> height;
	} else if (cmd == "maxdepth") {
		line >> maxdepth;
	} else if (cmd == "output") {
		line >> filename;
	} else if (cmd == "camera") {
		double arg1, arg2, arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		eye = vec3(arg1,arg2,arg3);
		line >> arg1;
		line >> arg2;
		line >> arg3;
		vec3 lookat = vec3(arg1,arg2,arg3);
		line >> arg1;
		line >> arg2;
		line >> arg3;
		vec3 up = vec3(arg1,arg2,arg3);
		setCoordinateFrame(lookat,up);
		line >> fovy;
	} else if (cmd == "sphere") {
		double arg1, arg2, arg3, arg4;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		line >> arg4;
		Sphere* s = new Sphere(vec3(arg1,arg2,arg3), arg4);
		s->ambient = ambient;
		objects.push_back(s);
	} else if (cmd == "maxverts") {
		line >> maxverts;
		verts.reserve(maxverts);
	} else if (cmd == "maxvertnorms") {
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
		Triangle* t = new Triangle(verts[a1],verts[a2],verts[a3]);
		t->ambient = ambient;
		objects.push_back(t);
	} else if(cmd == "trinormal") {
		double arg1,arg2,arg3,arg4,arg5,arg6;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		line >> arg4;
		line >> arg5;
		line >> arg6;
		//NormTriangle* t = new NormTriangle(arg1,arg2,arg3,arg4,arg5,arg6);
		//objects.push_back(t);
	} else if(cmd == "translate") {
		double arg1,arg2,arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		mv.top() = mv.top() * Transform::translate(arg1, arg2, arg3);
	} else if(cmd == "rotate") {
		double arg1,arg2,arg3,arg4;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		line >> arg4;
		mv.top() = mv.top() * mat4(Transform::rotate(arg4,vec3(arg1,arg2,arg3)));
	} else if (cmd=="scale") {
		double arg1,arg2,arg3;
		line >> arg1;
		line >> arg2;
		line >> arg3;
		mv.top() = mv.top() * Transform::translate(arg1, arg2, arg3);
	} else if (cmd == "pushTransform") {
		mv.push(mv.top());
	} else if (cmd == "popTransform"){
		mv.pop();
	} else if (cmd == "directional") {
		//implement me
	} else if (cmd == "point") {
		//implement me
	} else if (cmd == "attenuation") {
		//implement me
	} else if (cmd == "ambient") {
		double arg1, arg2, arg3;
		line>>arg1;
		line>>arg2;
		line>>arg3;
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
		double arg1;
		line>>arg1;
		shininess = arg1;
	} else if (cmd == "emission") {
		double arg1, arg2, arg3;
		line>>arg1;
		line>>arg2;
		line>>arg3;
		emission = vec3(arg1,arg2,arg3);
	}
	cout << cmd << endl;//print command name while parsing
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