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
vec3 center;	// center of the world, usually (0,0,0)
bool useGlu; 	// Toggle use of "official" opengl/glm transform vs user code
bool useLights; // Toggle lights on and off
int wid, high;    	// width and height 
GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale, light} transop ; // which operation to transform by 
enum oper {amb,diff,spec,emis,shin,teapot,sphere,cube,trans,rot,scal,push,pop};
float sx, sy ; // the scale in x and y 
float tx, ty ; // the translation in x and y
float fovy;    // field of view
vec4 light_position[MAXLIGHTS]; //current position of the 10 lights
vec4 light_position_init[MAXLIGHTS];  //initial position of lights
vec4 light_specular[MAXLIGHTS]; //color of lights
vec3 lightUp[MAXLIGHTS];	//up vectors for all the lights
int currentLight;	//current light being moved
int numLights;		//number of lights in the scene

/* Data structure for input command */
struct command {
	oper op;
	vec4 args;
};

/* List of commands ran every time drawObjects() is called */
std::vector<command> commands; 

/* Variables to set uniform params for lighting fragment shader */
GLuint islight ; 
GLuint numLightsShader;

GLuint ambient ; 
GLuint diffuse ; 
GLuint specular ; 
GLuint shininess ;
GLuint emission ;

GLuint lightPosn;
GLuint lightColor;


/* Uses the Projection matrices (technically deprecated) to set perspective 
   We could also do this in a more modern fashion with glm.	*/ 
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

/* Keyboard options */
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
		useLights = true;
		glUniform1i(islight, useLights) ;
		sx = sy = 1.0 ; 
		tx = ty = 0.0 ; 
		for(int i=0; i<numLights; i++) {
			light_position[i] = light_position_init[i];
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
	case 'l':
		useLights = !useLights;
		glUniform1i(islight, useLights) ;
		std::cout << "useLights is now set to" << (useLights ? " true " : " false ") << "\n" ;
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
		vec3 a = vec3(light_position[num]);
		vec3 side = glm::cross(a,up);
		lightUp[num] = glm::normalize(glm::cross(side,a));
		std::cout << "Now controlling light "<<num<<"\n";
		break;
	}
	glutPostRedisplay();
}

/*	You will need to enter code for the arrow keys 
    When an arrow key is pressed, it will call your transform functions */
void specialKey(int key, int x, int y) {
	switch(key) {
	case 100: //left
		  if (transop == view) Transform::left(amount, eye,	 up);
		  else if (transop == scale) sx -= amount * 0.01 ; 
		  else if (transop == translate) tx -= amount * 0.01 ;
		  else if (transop == light){
			vec3 a = vec3(light_position[currentLight]);
			Transform::left(amount, a, lightUp[currentLight]);
		  }
		  break;
	case 101: //up
		  if (transop == view) Transform::up(amount,  eye,	up);
		  else if (transop == scale) sy += amount * 0.01 ; 
		  else if (transop == translate) ty += amount * 0.01 ;
		  else if (transop == light){
			vec3 a = vec3(light_position[currentLight]);
			Transform::up(amount,a,lightUp[currentLight]);
		  }
		  break;
	case 102: //right
		  if (transop == view) Transform::left(-amount, eye,  up);
		  else if (transop == scale) sx += amount * 0.01 ; 
		  else if (transop == translate) tx += amount * 0.01 ; 
		  else if (transop == light){
			vec3 a = vec3(light_position[currentLight]);
			Transform::left(-amount,a,lightUp[currentLight]);
		  }
		  break;
	case 103: //down
		  if (transop == view) Transform::up(-amount,  eye,	 up);
		  else if (transop == scale) sy -= amount * 0.01 ; 
		  else if (transop == translate) ty -= amount * 0.01 ; 
		  else if (transop == light){
			vec3 a = vec3(light_position[currentLight]);
			Transform::up(-amount,a,lightUp[currentLight]);
		  }
		  break;
	}
	glutPostRedisplay();
}

/* Parses a line of input and takes appropriate action */
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
		line >> lookfromx >> lookfromy >> lookfromz >> lookatx >> 
			lookaty >> lookatz >> upx >> upy >> upz >> fovy;
		eyeinit = vec3(lookfromx,lookfromy,lookfromz);
		center = vec3(lookatx,lookaty,lookatz);
		upinit = vec3(upx,upy,upz);
	} else if(cmd == "light") {
		if(numLights > 9) {
			return;
		} else {
			line >> x >> y >> z >> w >> r >> g >> b >> a;
			light_position_init[numLights] = vec4(x,y,z,w);
			light_specular[numLights] = vec4(r,g,b,a);
			numLights++;
		}
	} else if(cmd == "ambient") {
		line >> r >> g >> b >> a;
		command com;
		com.op = amb;
		com.args = vec4(r,g,b,a);
		commands.push_back(com);
	} else if(cmd == "diffuse") {
		line >> r >> g >> b >> a;
		command com;
		com.op = diff;
		com.args = vec4(r,g,b,a);
		commands.push_back(com);
	} else if(cmd == "specular") {
		line >> r >> g >> b >> a;
		command com;
		com.op = spec;
		com.args = vec4(r,g,b,a);
		commands.push_back(com);
	} else if(cmd == "emission") {
		line >> r >> g >> b >> a;
		command com;
		com.op = emis;
		com.args = vec4(r,g,b,a);
		commands.push_back(com);
	} else if(cmd == "shininess") {
		line >> s;
		command com;
		com.op = shin;
		com.args = vec4(s,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "teapot") {
		line >> size;
		command com;
		com.op = teapot;
		com.args = vec4(size,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "sphere") {
		line >> size;
		command com;
		com.op = sphere;
		com.args = vec4(size,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "cube") {
		line >> size;
		command com;
		com.op = cube;
		com.args = vec4(size,0.0,0.0,0.0);
		commands.push_back(com);
	} else if(cmd == "translate") {
		line >> x >> y >> z;
		command com;
		com.op = trans;
		com.args = vec4(x,y,z,0.0);
		commands.push_back(com);
	} else if(cmd == "rotate") {
		line >> x >> y >> z >> theta;
		command com;
		com.op = rot;
		com.args = vec4(x,y,z,theta);
		commands.push_back(com);
	} else if(cmd == "scale") {
		line >> x >> y >> z;
		command com;
		com.op = scal;
		com.args = vec4(x,y,z,0.0);
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
void parse(char* filename) {
	std::ifstream myfile(filename,std::ifstream::in);
	std::string line;
	if(myfile.is_open()) {
		while(myfile.good()) {
			getline(myfile, line);
			parseLine(line);
		}
	} else { 
		std::cerr << "Unable to open file " << filename << std::endl;
		exit(1);
	}
}

/* Default values so the program doesn't crash with empty input */
void def() {
	numLights = 0;
	fovy = 90;
	wid = 600;
	high = 400;
	eyeinit = vec3(0,0,5);
	center = vec3(0,0,0);
	upinit = vec3(0,1,0);
}

void init() {

	eye = eyeinit ; 
	up = upinit ;
	amount = 5;
	sx = sy = 1.0 ; 
	tx = ty = 0.0 ;
	useGlu = false;
	useLights = true;
	
	for(int i=0; i<numLights; i++) {
		light_position[i] = light_position_init[i];
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
	
	lightPosn = glGetUniformLocation(shaderprogram,"lightPosn");
	lightColor = glGetUniformLocation(shaderprogram,"lightColor");
	
	/* Set variables that don't change during simulation */
	glUniform1i(islight, useLights) ;
	glUniform1i(numLightsShader, numLights);
	
	glUniform4fv(lightColor, MAXLIGHTS, (GLfloat*)&light_specular[0]);
	
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
				glUniform4fv(ambient,1,(GLfloat*)&com.args[0]);
				break;
			case diff:
				glUniform4fv(diffuse,1,(GLfloat*)&com.args[0]);
				break;
			case spec:
				glUniform4fv(specular,1,(GLfloat*)&com.args[0]);
				break;
			case emis:
			 	glUniform4fv(emission,1,(GLfloat*)&com.args[0]);
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
				//transf = mat4(Transform::rotate(com.args[3], axis));
				transf = mat4(Transform::rotate(com.args[3], vec3(com.args)));
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

/* main display */
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
	
	vec4 light[MAXLIGHTS];
	for (int i=0; i<numLights; i++){
		light[i] = mv * light_position[i];
	}
	glUniform4fv(lightPosn, MAXLIGHTS, (GLfloat*)&light[0]);
	
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
	def();
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
