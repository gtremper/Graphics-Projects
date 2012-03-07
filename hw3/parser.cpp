#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include <map>
#include <GLUT/glut.h>
#include "Transform.h"
#include "globals.h"

// COLORS: sky: (135/225.0, 206/255.0, 0) forest green: (34/255.0, 139/255.0, 34/255.0)

#define BUFFER_OFFSET(bytes) ((GLubyte *) NULL + (bytes))
#define NumberOf(array) (sizeof(array)/sizeof(array[0]))

using namespace std;

map<string,int> modelMap;
int numObjects=0;


/* Parses a line of input and takes appropriate action */
void parseLine(string l, vector<command> &commands) {
	float arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
	stringstream line(l);
	string cmd;
	line >> cmd;
	if(cmd[0] == '#') { // comment
		return;
	} else if(cmd == "") { // blank line
		return;
	} else if(cmd == "size") {
		line >> width >> height;
	} else if(cmd == "camera") {
		line >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> fovy;
		eyeinit = vec3(arg1,arg2,arg3);
		yawInit = arg4;
		pitchInit = arg5;
	} else if(cmd == "light") {
		if(numLights > 9) {
			return;
		} else {
			line >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> arg6 >> arg7 >> arg8;
			light_position[numLights] = vec4(arg1,arg2,arg3,arg4);
			light_specular[numLights] = vec4(arg5,arg6,arg7,arg8);
			numLights++;
		}
	} else if(cmd == "ambient") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = amb;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "diffuse") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = diff;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "specular") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = spec;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "emission") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //r g b a
		command com;
		com.op = emis;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "shininess") {
		line >> arg1; //s
		command com;
		com.op = shin;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "teapot") {
		line >> arg1; //size
		command com;
		com.op = teapot;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "sphere") {
		line >> arg1; //size
		command com;
		com.op = sphere;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "cube") {
		line >> arg1; //size
		command com;
		com.op = cube;
		com.args = vec4(arg1,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "translate") {
		line >> arg1 >> arg2 >> arg3; //x y z
		command com;
		com.op = trans;
		com.args = vec4(arg1,arg2,arg3,0.0);
		commands.push_back(com);
	} else if(cmd == "rotate") {
		line >> arg1 >> arg2 >> arg3 >> arg4; //x y z theta
		command com;
		com.op = rot;
		com.args = vec4(arg1,arg2,arg3,arg4);
		commands.push_back(com);
	} else if(cmd == "scale") {
		line >> arg1 >> arg2 >> arg3; //x y z
		command com;
		com.op = scal;
		com.args = vec4(arg1,arg2,arg3,0.0);
		commands.push_back(com);
	} else if(cmd == "pushTransform") {
		command com;
		com.op = push;
		commands.push_back(com);
	} else if(cmd == "popTransform") {
		command com;
		com.op = pop;
		commands.push_back(com);
	}
}

/* Parse the whole file */
vector<command> parseInput(char* filename) {
	ifstream myfile(filename,ifstream::in);
	string line;
	vector<command> commands;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			parseLine(line,commands);
		}
	} else { 
		cerr << "Unable to open file " << filename << endl;
		exit(1);
	}
	myfile.close();
	return commands;
}


void parseOBJ(string filename){
	vector<vec3> v; // vectors
	vector<vec3> f; // faces
	ifstream myfile(filename.c_str(), ifstream::in);
	if(myfile.is_open()) {
		while(myfile.good()) {
			string line;
			string cmd;
			getline(myfile, line);
			stringstream ln(line);
			ln >> cmd;
			if(cmd == "v") {
				float x,y,z;
				ln >> x >> y >> z;
				vec3 p(x, y, z);
				v.push_back(p);
			}
			if(cmd == "f") {
				int v1, v2, v3;
				ln >> v1 >> v2 >> v3;
				vec3 t(v1-1, v2-1, v3-1);
				f.push_back(t);
			}
		}
	} else {
		cout << "Unable to open file " << filename << endl;
	}
	numObjects++;
	 
}






void loadObjects(char* filename) {
	ifstream myfile(filename,ifstream::in);
	string line;
	vector<string> modelFiles;
	int modelNum = 0;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			stringstream path("data/");
			path << line;
			modelMap[line] = modelNum;
			modelNum++;
			modelFiles.push_back(path.str());
		}
	} else { 
		cerr << "Unable to open file " << filename << endl;
		exit(1);
	}
	myfile.close();
	
	for (int i=0; i<modelNum; i++){
		string file = modelFiles[i];
		string extension = file.substr(file.find_last_of(".") + 1);
		if(extension == "obj") {
			parseOBJ(file);
		} else if (extension == "raw") {
			//run raw parser
		} else {
			cerr << "The extension \'" << extension <<"\' is not supported." << endl;
			exit(1);
		}
	}
}















