/*************************************************************************/
/*   This is a scene viewer written by Graham Tremper and Wesley Prager  */
/*************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <stack>
#include <map>
#include <vector>
#include <string>
#include <GLUT/glut.h>
#include "shaders.h"
#include "Transform.h"

#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))

using namespace std;

const int MAXLIGHTS = 10;
const float WALKSPEED = 0.5;
const float SENSITIVITY = 0.3;
const vec3 UP = vec3(0.0,1.0,0.0);
const vec3 FORWARD = vec3(0.0,0.0,-1.0);
const vec3 SIDE = vec3(1.0,0.0,0.0);

vec3 eye; 		// The (regularly updated) vector coordinates of the eye location 
float pitch;
float yaw;
vec3 eyeinit;	// Initial eye position, also for resets
float pitchInit;
float yawInit;

bool useLights; // Toggle light shading on and off
bool useTex; // Toggle if using a texture for this object
bool passive; // Toggle passive mouse movement
int width, height;  
GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
bool flyMode;
float fovy;    // field of view
vec4 light_position[MAXLIGHTS]; //current position of the 10 lights
vec4 light_specular[MAXLIGHTS]; //color of lights
int numLights;
int lastx, lasty ; // For mouse motion

float teapotangle = 0.0;
int animate = 0;

std::vector<command> commands; 


/* Forward Declaration */
std::vector<command> parseInput(char*);

void loadObjects(char*);
void draw(int);




/* Variables to set uniform params for lighting fragment shader */
GLuint islight ;
GLuint istex;
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
void reshape(int w, int h){
	width = w;
	height = h;
	mat4 mv ; // just like for lookat

	glMatrixMode(GL_PROJECTION);
	float newFovy = fovy; 
	float aspect = width / (float) height, zNear = 0.1, zFar = 99.0 ;

	mv = Transform::perspective(newFovy,aspect,zNear,zFar) ; 
	mv = glm::transpose(mv) ; // accounting for row major 
	glLoadMatrixf(&mv[0][0]) ; 

	glViewport(0, 0, width, height);
}


void printHelp() {
  std::cout << "\npress 'h' to print this message again.\n" 
			<< "press 'f' to switch to fly mode.\n"
			<< "press 'p' to begin the animation.\n"
			<< "press 'l' to toggle lights and textures.\n"
			<< "press ESC to quit.\n";
	
}

/* Mouse Functions */

void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			lastx = x ; lasty = y ;
			passive = true;
    	} else if (state == GLUT_DOWN) {
			passive = false;
		}
	}
}

void mouse(int x, int y) {
	
	if(!passive) return;
	
	int diffx=x-lastx; 
    int diffy=y-lasty; 
    lastx=x; //set lastx to the current x position
    lasty=y; //set lasty to the current y position
	
    yaw -= diffx*SENSITIVITY; 
    pitch -= diffy*SENSITIVITY;

	if (pitch>80) pitch = 80;
	if (pitch<-80) pitch = -80;
}


/* Keyboard options */
void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 'w': //up
		eye += WALKSPEED*Transform::direction(yaw,pitch,FORWARD,UP,flyMode);
		break;
	case 's': //down
		eye -= WALKSPEED*Transform::direction(yaw,pitch,FORWARD,UP,flyMode);
		break;	
	case 'a': //left
		eye -= WALKSPEED*Transform::direction(yaw,pitch,SIDE,UP,false);
		break;
	case 'd': //right
		eye += WALKSPEED*Transform::direction(yaw,pitch,SIDE,UP,false);;
		break;
	case 'h':
		printHelp();
		break;
	case 27:  // Escape to quit
			exit(0) ;
			break ;
	case 'f': // switch between fly and no fly 
		eye = eyeinit ; 
		pitch = pitchInit;
		yaw = yawInit;
		useLights = true;
		flyMode = !flyMode;
		glUniform1i(islight, useLights) ;
		break ;	  
	case 'l':
		useLights = !useLights;
		glUniform1i(islight, useLights) ;
		std::cout << "useLights is now set to" << (useLights ? " true " : " false ") << "\n" ;
		break; 
	case 'p': // to pause/restart animation
		animate = !animate;
		break;
	}
	glutPostRedisplay();
}


/* Default values so the program doesn't crash with empty input */
void def() {
	numLights = 0;
	fovy = 90;
	width = 600;
	height = 400;
	eyeinit = vec3(0,0,1);
	pitch = 0.0;
	yaw = 0.0;
}

void init() {

	eye = eyeinit ; 
	yaw = yawInit;
	pitch = pitchInit;
	useLights = true;
	useTex = false;
	flyMode = false;
	passive = true;
	lastx = width/2;
	lasty = height/2;

	glEnable(GL_DEPTH_TEST);

	vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl");
	fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl");
	shaderprogram = initprogram(vertexshader, fragmentshader);
	istex = glGetUniformLocation(shaderprogram,"istex");
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
	
	glUniform1i(istex,false);
	GLint texsampler ; 
	texsampler = glGetUniformLocation(shaderprogram, "tex") ; 
	glUniform1i(texsampler,0) ; // Could also be GL_TEXTURE0
	
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
			case anim:
				transf = mv*matStack.top();
				glLoadMatrixf(&transf[0][0]);
				glRotatef(teapotangle*com.args[1], 0.0, 1.0, 0.0);
				glutSolidTeapot(com.args[0]);
				// transf = mat4(Transform::rotate(teapotangle*com.args[1], UP));
				// transf = glm::transpose(transf);
				// transf = matStack.top()*transf;
			default:
				transf = mv*matStack.top();
				glLoadMatrixf(&transf[0][0]);
				draw(-com.op-1); // negative so doesnt conflict with enum
				break;
		}	
	}
}

void animation(void) {
	teapotangle = teapotangle + 1.0;
}

/* main display */
void display() {
	glClearColor(135/225.0, 206/255.0, 250/255.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	
	mat4 mv; 
	vec3 dir = Transform::direction(yaw,pitch,FORWARD,UP,true);
	mv = Transform::lookAt(eye,eye+dir,UP) ; 
	mv = glm::transpose(mv) ; // accounting for row major	
	glLoadMatrixf(&mv[0][0]) ; 
	
	vec4 light[MAXLIGHTS];
	for (int i=0; i<numLights; i++){
		light[i] = mv * light_position[i];
	}
	glUniform4fv(lightPosn, MAXLIGHTS, (GLfloat*)&light[0]);
	
	drawObjects(commands,mv);	
	
	if(animate) {
		animation();
	}
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	def();
	if (argc != 3) {
		std::cerr << "You need 2 text file as the arguments\n Scene then models\n";
		exit(1);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("HW3: Hedge Maze");
	loadObjects(argv[2]);
	commands = parseInput(argv[1]);
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutReshapeWindow(width,height);
	glutIdleFunc (display);
	glutPassiveMotionFunc(mouse);
	glutMouseFunc(mouseClick) ;
	printHelp();
	glutMainLoop();
	return 0;
}
