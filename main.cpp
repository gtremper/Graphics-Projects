/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects	 */
/*****************************************************************************/


#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include <GLUT/glut.h>
#include "shaders.h"
#include "Transform.h"

const int MAXLIGHTS = 10;

int amount; 	// The amount of rotation for each arrow press

vec3 eye; 		// The (regularly updated) vector coordinates of the eye location 
vec3 up;		// The (regularly updated) vector coordinates of the up location 
vec3 eyeinit;	// Initial eye position, also for resets
vec3 upinit; 	// Initial up position, also for resets
vec3 center;
bool useGlu; 	// Toggle use of "official" opengl/glm transform vs user code
int wid, high;    	// width and height 
GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale, light} transop ; // which operation to transform by 
enum oper {amb,diff,spec,emis,shin,teapot,sphere,cube,trans,rot,scal,push,pop};
float sx, sy ; // the scale in x and y 
float tx, ty ; // the translation in x and y
float fovy;
GLfloat light_position[10][4];
vec4 light_position_init[10];
GLfloat light_specular[10][4];
vec3 lightUp[10];
int currentLight;
int numLights;

/* Data structure for input command */
struct command {
	oper op;
	GLfloat args[4];
};
std::vector<command> commands;


// Variables to set uniform params for lighting fragment shader 
GLuint islight ; 
GLuint numLightsShader;

GLuint ambient ; 
GLuint diffuse ; 
GLuint specular ; 
GLuint shininess ;
GLuint emission ;

GLuint lightPosn[MAXLIGHTS];
GLuint lightColor[MAXLIGHTS];


// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
void transformvec (GLfloat input[4], GLfloat output[4]) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 
  
  for (int i = 0 ; i < 4 ; i++) {
	output[i] = 0 ; 
	for (int j = 0 ; j < 4 ; j++) 
	  output[i] += modelview[4*j+i] * input[j] ; 
  }
}

// Uses the Projection matrices (technically deprecated) to set perspective 
// We could also do this in a more modern fashion with glm.	 
void reshape(int width, int height){
	wid = width;
	high = height;
	mat4 mv ; // just like for lookat

	glMatrixMode(GL_PROJECTION);
	float newFovy = fovy; 
	float aspect = wid / (float) high, zNear = 0.1, zFar = 99.0 ;
	// I am changing the projection stuff to be consistent with lookat
	if (useGlu) mv = glm::perspective(newFovy,aspect,zNear,zFar) ; 
	else {
		  mv = Transform::perspective(newFovy,aspect,zNear,zFar) ; 
		  mv = glm::transpose(mv) ; // accounting for row major 
		}
	glLoadMatrixf(&mv[0][0]) ; 

	glViewport(0, 0, wid, high);
}



void printHelp() {
  std::cout << "\npress 'h' to print this message again.\n" 
	   << "press '+' or '-' to change the amount of rotation that\noccurs with each arrow press.\n" 
			<< "press 'g' to switch between using glm::lookAt and glm::Perspective or your own LookAt.\n"		
			<< "press 'r' to reset the transformations.\n"
			<< "press 'v' 't' 's' to do view [default], translate, scale.\n"
			<< "press ESC to quit.\n" ;	 
	
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case '+':
		amount++;
		std::cout << "amount set to " << amount << "\n" ;
		break;
	case '-':
		amount--;
		std::cout << "amount set to " << amount << "\n" ; 
		break;
	case 'g':
		useGlu = !useGlu;
		reshape(wid,high) ; 
		std::cout << "Using glm::LookAt and glm::Perspective set to: " << (useGlu ? " true " : " false ") << "\n" ; 
		break;
	case 'h':
		printHelp();
		break;
	case 27:  // Escape to quit
			exit(0) ;
			break ;
	case 'r': // reset eye and up vectors, scale and translate. 
		eye = eyeinit ; 
		up = upinit ; 
		sx = sy = 1.0 ; 
		tx = ty = 0.0 ; 
		for(int i=0; i<numLights; i++) {
			light_position[i][0] = light_position_init[i][0];
			light_position[i][1] = light_position_init[i][1];
			light_position[i][2] = light_position_init[i][2];
			light_position[i][3] = light_position_init[i][3];
		}
		break ;	  
	case 'v': 
		transop = view ;
		std::cout << "Operation is set to View\n" ; 
		break ; 
	case 't':
		transop = translate ; 
		std::cout << "Operation is set to Translate\n" ;
		break ; 
	case 's':
		transop = scale ; 
		std::cout << "Operation is set to Scale\n" ;
		break;
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
		int num = key-48;
		if (num>=numLights){
			std::cout<<"That light does not exist\n";
			break;
		}
		transop = light;
		currentLight = num;
		vec3 a(light_position[num][0],light_position[num][1],light_position[num][2]);
		vec3 side = glm::cross(a,up);
		lightUp[num] = glm::cross(side,a);
		std::cout << "Now controlling light "<<num<<"\n";
		break;
	
	}
	glutPostRedisplay();
}

//	You will need to enter code for the arrow keys 
//	When an arrow key is pressed, it will call your transform functions

void specialKey(int key, int x, int y) {
	switch(key) {
	case 100: //left
		  if (transop == view) Transform::left(amount, eye,	 up);
		  else if (transop == scale) sx -= amount * 0.01 ; 
		  else if (transop == translate) tx -= amount * 0.01 ;
		  else if (transop == light){
			vec3 a(light_position[currentLight][0],light_position[currentLight][1],light_position[currentLight][2]);
			Transform::left(amount,a,lightUp[currentLight]);
			light_position[currentLight][0] = a[0];
			light_position[currentLight][1] = a[1];
			light_position[currentLight][2] = a[2];
		  }
		  break;
	case 101: //up
		  if (transop == view) Transform::up(amount,  eye,	up);
		  else if (transop == scale) sy += amount * 0.01 ; 
		  else if (transop == translate) ty += amount * 0.01 ;
		  else if (transop == light){
			vec3 a(light_position[currentLight][0],light_position[currentLight][1],light_position[currentLight][2]);
			Transform::up(amount,a,lightUp[currentLight]);
			light_position[currentLight][0] = a[0];
			light_position[currentLight][1] = a[1];
			light_position[currentLight][2] = a[2];
		  }
		  break;
	case 102: //right
		  if (transop == view) Transform::left(-amount, eye,  up);
		  else if (transop == scale) sx += amount * 0.01 ; 
		  else if (transop == translate) tx += amount * 0.01 ; 
		  else if (transop == light){
			vec3 a(light_position[currentLight][0],light_position[currentLight][1],light_position[currentLight][2]);
			Transform::left(-amount,a,lightUp[currentLight]);
			light_position[currentLight][0] = a[0];
			light_position[currentLight][1] = a[1];
			light_position[currentLight][2] = a[2];
		  }
		  break;
	case 103: //down
		  if (transop == view) Transform::up(-amount,  eye,	 up);
		  else if (transop == scale) sy -= amount * 0.01 ; 
		  else if (transop == translate) ty -= amount * 0.01 ; 
		  else if (transop == light){
			vec3 a(light_position[currentLight][0],light_position[currentLight][1],light_position[currentLight][2]);
			Transform::up(-amount,a,lightUp[currentLight]);
			light_position[currentLight][0] = a[0];
			light_position[currentLight][1] = a[1];
			light_position[currentLight][2] = a[2];
		  }
		  break;
	}
	glutPostRedisplay();
}


void parseLine(std::string l) {
	float lookfromx, lookfromy, lookfromz, lookatx, lookaty, lookatz, upx, upy, upz;
	float x, y, z, w, r, g, b, a, s, size, theta;
	std::stringstream line(l);
	std::string cmd;
	line >> cmd;
	if(cmd[0] == '#') { // comment
		return;
	} else if(cmd == "") { // blank line
		return;
	} else if(cmd == "size") {
		line >> wid >> high;
	} else if(cmd == "camera") {
		line >> lookfromx >> lookfromy >> lookfromz >> lookatx >> lookaty >> lookatz >> upx >> upy >> upz >> fovy;
		eyeinit = vec3(lookfromx,lookfromy,lookfromz);
		center = vec3(lookatx,lookaty,lookatz);
		upinit = vec3(upx,upy,upz);
	} else if(cmd == "light") {
		if(numLights > 9) {
			return;
		} else {
			line >> x >> y >> z >> w >> r >> g >> b >> a;
			light_position_init[numLights] = vec4(x,y,z,w);
			light_specular[numLights][0] = r;
			light_specular[numLights][1] = g;
			light_specular[numLights][2] = b;
			light_specular[numLights][3] = a;
			numLights++;
		}
	} else if(cmd == "ambient") {
		line >> r >> g >> b >> a;
		command com;
		com.op = amb;
		com.args[0] = r;
		com.args[1] = g;
		com.args[2] = b;
		com.args[3] = a;
		commands.push_back(com);
	} else if(cmd == "diffuse") {
		line >> r >> g >> b >> a;
		command com;
		com.op = diff;
		com.args[0] = r;
		com.args[1] = g;
		com.args[2] = b;
		com.args[3] = a;
		commands.push_back(com);
	} else if(cmd == "specular") {
		line >> r >> g >> b >> a;
		command com;
		com.op = spec;
		com.args[0] = r;
		com.args[1] = g;
		com.args[2] = b;
		com.args[3] = a;
		commands.push_back(com);
	} else if(cmd == "emission") {
		line >> r >> g >> b >> a;
		command com;
		com.op = emis;
		com.args[0] = r;
		com.args[1] = g;
		com.args[2] = b;
		com.args[3] = a;
		commands.push_back(com);
	} else if(cmd == "shininess") {
		line >> s;
		command com;
		com.op = shin;
		com.args[0] = s;
		commands.push_back(com);
	} else if(cmd == "teapot") {
		line >> size;
		command com;
		com.op = teapot;
		com.args[0] = size;
		commands.push_back(com);
	} else if(cmd == "sphere") {
		line >> size;
		command com;
		com.op = sphere;
		com.args[0] = size;
		commands.push_back(com);
	} else if(cmd == "cube") {
		line >> size;
		command com;
		com.op = cube;
		com.args[0] = size;
		commands.push_back(com);
	} else if(cmd == "translate") {
		line >> x >> y >> z;
		command com;
		com.op = trans;
		com.args[0] = x;
		com.args[1] = y;
		com.args[2] = z;
		commands.push_back(com);
	} else if(cmd == "rotate") {
		line >> x >> y >> z >> theta;
		command com;
		com.op = rot;
		com.args[0] = x;
		com.args[1] = y;
		com.args[2] = z;
		com.args[3] = theta;
		commands.push_back(com);
	} else if(cmd == "scale") {
		line >> x >> y >> z;
		command com;
		com.op = scal;
		com.args[0] = x;
		com.args[1] = y;
		com.args[2] = z;
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

void parse(char* filename) {
	numLights = 0;
	std::ifstream myfile(filename,std::ifstream::in);
	std::string line;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			parseLine(line);
		}
	}
	else std::cout << "Unable to open file " << filename << std::endl;
}

void init() {

	eye = eyeinit ; 
	up = upinit ;
	amount = 5;
	sx = sy = 1.0 ; 
	tx = ty = 0.0 ;
	useGlu = false;
	
	for(int i=0; i<numLights; i++) {
		light_position[i][0] = light_position_init[i][0];
		light_position[i][1] = light_position_init[i][1];
		light_position[i][2] = light_position_init[i][2];
		light_position[i][3] = light_position_init[i][3];
	}

	glEnable(GL_DEPTH_TEST);

	vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl");
	fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl");
	shaderprogram = initprogram(vertexshader, fragmentshader);
	islight = glGetUniformLocation(shaderprogram,"islight");
	numLightsShader = glGetUniformLocation(shaderprogram,"numLights");
	
	ambient = glGetUniformLocation(shaderprogram,"ambient");
	diffuse = glGetUniformLocation(shaderprogram,"diffuse");
	specular = glGetUniformLocation(shaderprogram,"specular");
	shininess = glGetUniformLocation(shaderprogram,"shininess");
	emission = glGetUniformLocation(shaderprogram,"emission");
	
	lightPosn[0] = glGetUniformLocation(shaderprogram,"lightPosn[0]");
	lightColor[0] = glGetUniformLocation(shaderprogram,"lightColor[0]");
	lightPosn[1] = glGetUniformLocation(shaderprogram,"lightPosn[1]");
	lightColor[1] = glGetUniformLocation(shaderprogram,"lightColor[1]");
	lightPosn[2] = glGetUniformLocation(shaderprogram,"lightPosn[2]");
	lightColor[2] = glGetUniformLocation(shaderprogram,"lightColor[2]");
	lightPosn[3] = glGetUniformLocation(shaderprogram,"lightPosn[3]");
	lightColor[3] = glGetUniformLocation(shaderprogram,"lightColor[3]");
	lightPosn[4] = glGetUniformLocation(shaderprogram,"lightPosn[4]");
	lightColor[4] = glGetUniformLocation(shaderprogram,"lightColor[4]");
	lightPosn[5] = glGetUniformLocation(shaderprogram,"lightPosn[5]");
	lightColor[5] = glGetUniformLocation(shaderprogram,"lightColor[5]");
	lightPosn[6] = glGetUniformLocation(shaderprogram,"lightPosn[6]");
	lightColor[6] = glGetUniformLocation(shaderprogram,"lightColor[6]");
	lightPosn[7] = glGetUniformLocation(shaderprogram,"lightPosn[7]");
	lightColor[7] = glGetUniformLocation(shaderprogram,"lightColor[7]");
	lightPosn[8] = glGetUniformLocation(shaderprogram,"lightPosn[8]");
	lightColor[8] = glGetUniformLocation(shaderprogram,"lightColor[8]");
	lightPosn[9] = glGetUniformLocation(shaderprogram,"lightPosn[9]");
	lightColor[9] = glGetUniformLocation(shaderprogram,"lightColor[9]");
	
	
	/* Set variables that don't change during simulation */
	glUniform1i(islight, true) ;
	glUniform1i(numLightsShader, numLights);
	
	GLfloat light[4];
	for (int i=0; i<numLights; i++){
		glUniform4fv(lightColor[i], 1, light_specular[i]);	
	}
	
}

/* Draws objects based on list of commands */
void drawObjects(std::vector<command> comms, mat4 mv) {
	std::stack<mat4> matStack;
	matStack.push(mat4(1.0));
	
	std::vector<command>::iterator it;
	for(it = comms.begin(); it<comms.end(); it++){
		
		command com = *it;
		mat4 transf , sc , tr;
		vec3 axis;
		switch(com.op) {
			case amb:
				glUniform4fv(ambient,1,com.args);
				break;
			case diff:
				glUniform4fv(diffuse,1,com.args);
				break;
			case spec:
				glUniform4fv(specular,1,com.args);
				break;
			case emis:
			 	glUniform4fv(emission,1,com.args);
				break;
			case shin:
				glUniform1f(shininess,com.args[0]);
				break;
			case teapot:
				transf = mv*matStack.top();
				glLoadMatrixf(&transf[0][0]);
				glutSolidTeapot(com.args[0]);
				break;
			case sphere:
				transf = mv*matStack.top();
				glLoadMatrixf(&transf[0][0]);
				glutSolidSphere(com.args[0],20,20);
				break;
			case cube:
				transf = mv*matStack.top();
				glLoadMatrixf(&transf[0][0]);
				glutSolidCube(com.args[0]);
				break;
			case trans:
				transf = Transform::translate(com.args[0],com.args[1],com.args[2]);
				transf = glm::transpose(transf);
				matStack.top() = matStack.top()*transf;
				break;
			case rot:
				axis = vec3(com.args[0],com.args[1],com.args[2]);
				transf = mat4(Transform::rotate(com.args[3], axis));
				transf = glm::transpose(transf);
				matStack.top() = matStack.top()*transf;
				break;
			case scal:
				transf = Transform::scale(com.args[0],com.args[1],com.args[2]);
				transf = glm::transpose(transf);
				matStack.top() = matStack.top()*transf;
				break;
			case push:
				transf = matStack.top();
				matStack.push(transf);
				break;
			case pop:
				matStack.pop();
				break;
		}
		
	}
}

void display() {
	glClearColor(0, 0, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	mat4 mv; 

	if (useGlu) mv = glm::lookAt(eye,center,up) ; 
	else {
		  mv = Transform::lookAt(eye,center,up) ; 
		  mv = glm::transpose(mv) ; // accounting for row major
		}
	glLoadMatrixf(&mv[0][0]) ; 
	
	GLfloat light[4];
	for (int i=0; i<numLights; i++){
		transformvec(light_position[i], light);
		std::cout << "Light "<<i<<": ("<<light[0]<<", "<<light[1]<<", "<<light[2]<<")\n";
		glUniform4fv(lightPosn[i], 1, light);
	}
	
	// Transformations for Teapot, involving translation and scaling 
	mat4 sc, tr; 
	sc = Transform::scale(sx,sy,1.0) ; 
	tr = Transform::translate(tx,ty,0.0) ; 
	// Multiply the matrices, accounting for OpenGL and GLM.
	sc = glm::transpose(sc) ; tr = glm::transpose(tr) ; 
	mat4 transf	 = mv * tr * sc ; // scale, then translate, then lookat.
		
	drawObjects(commands,transf);
		
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cerr << "You need a text file as the argument\n";
		exit(1);
	}
	parse(argv[1]);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("HW2: Scene Viewer");
	init();
	glutDisplayFunc(display);
	glutSpecialFunc(specialKey);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutReshapeWindow(wid,high);
	printHelp();
	glutMainLoop();
	return 0;
}
