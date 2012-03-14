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
#include "FreeImage.h"

using namespace std;

bool loadBitmap(string filename, FIBITMAP* &bitmap);
bool loadTexture(string filename, GLuint &texture);

// COLORS: sky: (135/225.0, 206/255.0, 250/255) forest green: (34/255.0, 139/255.0, 34/255.0)

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
#define NumberOf(array) (sizeof(array)/sizeof(array[0]))

map<string,int> modelMap;

const int MAXOBJECTS = 10;

GLuint objects[MAXOBJECTS][2]; // Object buffer IDs

int size[MAXOBJECTS]; //number of indicies
GLenum primType[MAXOBJECTS]; // primative type
bool normals[MAXOBJECTS]; //Does the object have normals?
int textures[MAXOBJECTS]; //Does the object have textures, if so which?
GLuint texBuff[3]; //Texture buffers. Change size if adding textures


struct vertex {
	GLfloat x, y, z; //position
	GLfloat nx, ny, nz; //normal
	GLfloat tx, ty; // texture coordinates
};



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
	} else if(cmd == "anim") {
		line >> arg1 >> arg2; //size speed
		command com;
		com.op = anim;
		com.args = vec4(arg1,arg2,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "constanim") {
		line >> arg1 >> arg2; //size speed
		command com;
		com.op = constanim;
		com.args = vec4(arg1,arg2,0.0,0.0);
		commands.push_back(com);
	}	else if(modelMap.find(cmd) != modelMap.end()) {
		command com;
		com.op = -modelMap[cmd]-1;
		commands.push_back(com);
	} else {
		cerr << "Command \""<< cmd <<"\" not supported\n";
		exit(1);
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

void parseRAW(string filename, int modelNum){
	ifstream myfile(filename.c_str(), ifstream::in);
	vector<float> v;
	int numVerts = 0;
	if(myfile.is_open()) {
		while(myfile.good()) {
			string line;
			getline(myfile,line);
			if(line[0] == '#'){
				continue;
			}
			stringstream ln(line);
			float vert;
			for(int i=0; i<9; i++){
				ln>>vert;
				v.push_back(vert);
				numVerts++;
			}
		}
	} else {
		cout << "Unable to open file " << filename << endl;
	}
	
	vertex verts[numVerts/3];
	GLushort inds[numVerts/3];
	for(int i=0; i<(numVerts/3); i++){
		verts[i].x = v[3*i];
		verts[i].y = v[3*i+1];
		verts[i].z = v[3*i+2];
		inds[i] = i;
	}
	
	glGenBuffers(2, &objects[modelNum][0]);
	glBindBuffer(GL_ARRAY_BUFFER, objects[modelNum][0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[modelNum][1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
	
	size[modelNum] = numVerts/3;
	primType[modelNum] = GL_TRIANGLES;
	normals[modelNum] = false;
	textures[modelNum] = -1 ;	
}


void parseOBJ(string filename, int modelNum){
	vector<vec3> v; // vectors
	vector<vec3> f; // faces
	ifstream myfile(filename.c_str(), ifstream::in);
	int numVerts = 0;
	int numFaces = 0;
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
				numVerts++;
			}
			if(cmd == "f") {
				int v1, v2, v3;
				ln >> v1 >> v2 >> v3;
				vec3 t(v1-1, v2-1, v3-1);
				f.push_back(t);
				numFaces++;
			}
		}
	} else {
		cout << "Unable to open file " << filename << endl;
	}
	vertex verts[numVerts];
	GLushort inds[numFaces*3];
	for(int i=0; i<numVerts; i++){
		verts[i].x = v[i][0];
		verts[i].y = v[i][1];
		verts[i].z = v[i][2];
	}
	for(int i=0; i<numFaces; i++){
		inds[3*i] = f[i][0];
		inds[3*i+1] = f[i][1];
		inds[3*i+2] = f[i][2];
	}
	glGenBuffers(2, &objects[modelNum][0]);
	glBindBuffer(GL_ARRAY_BUFFER, objects[modelNum][0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[modelNum][1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
	
	size[modelNum] = numFaces*3;
	primType[modelNum] = GL_TRIANGLES;
	normals[modelNum] = false;
	textures[modelNum] = -1 ;
}

int texCounter = 0;
void parseOBJ4(string filename, int modelNum){
	textures[modelNum] = -1;
	
	vector<vec3> v; // verticies
	vector<vec4> f; // faces
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
				int v1, v2, v3, v4;
				ln >> v1 >> v2 >> v3 >> v4;
				vec4 t(v1-1, v2-1, v3-1, v4-1);
				f.push_back(t);
			}
			if(cmd == "t") {
				string path;
				ln >> path;
				if ( loadTexture(path,texBuff[texCounter]) ){
					textures[modelNum] = texCounter;
					texCounter++;
					cout << "texture \""<<path <<"\" loaded" <<endl;
				} else {
					cerr << "Texture \""<< path <<"\" failed to load";
					exit(1);
				}
			}
		}
	} else {
		cout << "Unable to open file " << filename << endl;
	}
	int numFaces = f.size();
	vertex verts[numFaces*4];
	GLushort inds[numFaces*4];
	
	for(int i=0; i<numFaces; i++){
		vec4 face = f[i];
		vec3 a = v[face[1]]-v[face[0]];
		vec3 b = v[face[3]]-v[face[0]];
		vec3 normal = glm::normalize(glm::cross(a,b));
		
		float h = glm::length(a);
		float w = glm::length(b);
		w*=0.25;
		h*=0.25;
		float texcoordx[] = {w,w,0,0};
		float texcoordy[] = {0,h,h,0};
		
		for (int j=0; j<4; j++){
			verts[4*i+j].x = v[face[j]][0];
			verts[4*i+j].y = v[face[j]][1];
			verts[4*i+j].z = v[face[j]][2];
			verts[4*i+j].nx = normal[0];
			verts[4*i+j].ny = normal[1];
			verts[4*i+j].nz = normal[2];
			
			verts[4*i+j].tx = texcoordx[j];
			verts[4*i+j].ty = texcoordy[j];
		}
	}
	for(int i=0; i<numFaces*4; i++){
		inds[i]=i;
	}
	
	glGenBuffers(2, &objects[modelNum][0]);
	glBindBuffer(GL_ARRAY_BUFFER, objects[modelNum][0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[modelNum][1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(inds), inds, GL_STATIC_DRAW);
	
	size[modelNum] = numFaces*4;
	primType[modelNum] = GL_QUADS;
	normals[modelNum] = true;
}


void loadObjects(char* filename) {
	ifstream myfile(filename,ifstream::in);
	string line;
	vector<string> modelFiles;
	int modelNum = 0;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			stringstream path;
			path << "data/";
			path << line;
			modelMap[line] = modelNum;
			modelNum++;
			modelFiles.push_back(path.str());
		}
	} else { 
		cerr << "Unable to open file" << filename << endl;
		exit(1);
	}
	myfile.close();
	
	for (int i=0; i<modelNum; i++){
		string file = modelFiles[i];
		string extension = file.substr(file.find_last_of(".") + 1);
		if(extension == "obj") {
			parseOBJ(file,i);
		} else if (extension == "obj4"){
			parseOBJ4(file,i);
		} else if (extension == "raw") {
			parseRAW(file,i);
		} else {
			cerr << "The extension \'" << extension <<"\' is not supported." << endl;
			exit(1);
		}
	}
}

/* Draw object number "obj" */
void draw(int obj){
	glBindBuffer(GL_ARRAY_BUFFER, objects[obj][0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[obj][1]);
	
	glVertexPointer(3, GL_FLOAT, 32, BUFFER_OFFSET(0));
	glEnableClientState(GL_VERTEX_ARRAY);
	
	if(normals[obj]){
		glNormalPointer(GL_FLOAT, 32, BUFFER_OFFSET(12));
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	
	if(textures[obj] != -1 && useTex){
		glUniform1i(istex, true) ;
		glActiveTexture(GL_TEXTURE0) ; 
		glEnable(GL_TEXTURE_2D) ; 
		glBindTexture(GL_TEXTURE_2D, texBuff[textures[obj]]) ; 
		glEnableClientState(GL_TEXTURE_COORD_ARRAY) ; 
		glTexCoordPointer(2, GL_FLOAT, 32, BUFFER_OFFSET(24)) ;
	}
	if (obj==0 && wire) {
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	glDrawElements(primType[obj], size[obj], GL_UNSIGNED_SHORT, BUFFER_OFFSET(0)) ;
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glUniform1i(istex, false) ;
}


// load a bitmap with freeimage
bool loadBitmap(string filename, FIBITMAP* &bitmap) {
  // get the file format
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename.c_str(), 0);
  if (format == FIF_UNKNOWN)
    format = FreeImage_GetFIFFromFilename(filename.c_str());
  if (format == FIF_UNKNOWN)
    return false;

  // load the image
  bitmap = FreeImage_Load(format, filename.c_str());
  if (!bitmap)
    return false;

  return true;
}


// load a texture into opengl with freeimage
bool loadTexture(string filename, GLuint &texture) {
  FIBITMAP *bitmap = NULL;
  if (!loadBitmap(filename, bitmap)) {
    return false;
  }

  // convert to 32 bit bit-depth
  FIBITMAP *bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
  FreeImage_Unload(bitmap);
  if (!bitmap32) {
    return false;
  }
  bitmap = bitmap32;

  // get bits and dimensions
  BYTE *bits = FreeImage_GetBits(bitmap);
  int w = FreeImage_GetWidth(bitmap);
  int h = FreeImage_GetHeight(bitmap);

  // get bit order
  int order = GL_BGRA;

  // upload texture to opengl
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, order, GL_UNSIGNED_BYTE, (GLvoid*)bits);

  // forget our copy of the bitmap now that it's stored the card
  FreeImage_Unload(bitmap);

  return true;
}







