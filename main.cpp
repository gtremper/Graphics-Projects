/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/


#include <iostream>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"

int amount; // The amount of rotation for each arrow press

vec3 eye; // The (regularly updated) vector coordinates of the eye location 
vec3 up;  // The (regularly updated) vector coordinates of the up location 
const vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
const vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
bool useGlu; // Toggle use of "official" opengl/glm transform vs user code
int w, h; // width and height 
GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale} transop ; // which operation to transform by 
float sx, sy ; // the scale in x and y 
float tx, ty ; // the translation in x and y

// Constants to set up lighting on the teapot
const GLfloat light_position[] = {0, 5, 10, 1};    // Position of light 0
const GLfloat light_position1[] = {0, 5, -10, 1};  // Position of light 1
const GLfloat light_specular[] = {0.6, 0.3, 0, 1};    // Specular of light 0
const GLfloat light_specular1[] = {0, 0.3, 0.6, 1};   // Specular of light 1
const GLfloat one[] = {1, 1, 1, 1};                 // Specular on teapot
const GLfloat medium[] = {0.5, 0.5, 0.5, 1};        // Diffuse on teapot
const GLfloat small[] = {0.2, 0.2, 0.2, 1};         // Ambient on teapot
const GLfloat high[] = {100} ;                      // Shininess of teapot
GLfloat light0[4], light1[4] ; 

// Variables to set uniform params for lighting fragment shader 
GLuint islight ; 
GLuint light0posn ; 
GLuint light0color ; 
GLuint light1posn ; 
GLuint light1color ; 
GLuint ambient ; 
GLuint diffuse ; 
GLuint specular ; 
GLuint shininess ; 

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
void transformvec (const GLfloat input[4], GLfloat output[4]) {
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
	w = width;
	h = height;
        mat4 mv ; // just like for lookat

	glMatrixMode(GL_PROJECTION);
        float fovy = 90.0, aspect = w / (float) h, zNear = 0.1, zFar = 99.0 ;
        // I am changing the projection stuff to be consistent with lookat
        if (useGlu) mv = glm::perspective(fovy,aspect,zNear,zFar) ; 
        else {
          mv = Transform::perspective(fovy,aspect,zNear,zFar) ; 
          mv = glm::transpose(mv) ; // accounting for row major 
        }
        glLoadMatrixf(&mv[0][0]) ; 

	glViewport(0, 0, w, h);
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
                reshape(w,h) ; 
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
	}
	glutPostRedisplay();
}

//  You will need to enter code for the arrow keys 
//  When an arrow key is pressed, it will call your transform functions

void specialKey(int key, int x, int y) {
	switch(key) {
	case 100: //left
          if (transop == view) Transform::left(amount, eye,  up);
          else if (transop == scale) sx -= amount * 0.01 ; 
          else if (transop == translate) tx -= amount * 0.01 ; 
          break;
	case 101: //up
          if (transop == view) Transform::up(amount,  eye,  up);
          else if (transop == scale) sy += amount * 0.01 ; 
          else if (transop == translate) ty += amount * 0.01 ; 
          break;
	case 102: //right
          if (transop == view) Transform::left(-amount, eye,  up);
          else if (transop == scale) sx += amount * 0.01 ; 
          else if (transop == translate) tx += amount * 0.01 ; 
          break;
	case 103: //down
          if (transop == view) Transform::up(-amount,  eye,  up);
          else if (transop == scale) sy -= amount * 0.01 ; 
          else if (transop == translate) ty -= amount * 0.01 ; 
          break;
	}
	glutPostRedisplay();
}



void init() {
  
  // Set up initial position for eye, up and amount
  // As well as booleans 

        eye = eyeinit ; 
	up = upinit ; 
	amount = 5;
        sx = sy = 1.0 ; 
        tx = ty = 0.0 ;
	useGlu = false;

	glEnable(GL_DEPTH_TEST);

  // The lighting is enabled using the same framework as in mytest 3 
  // Except that we use two point lights
  // For now, lights and materials are set in display.  Will move to init 
  // later, per update lights

      vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
      fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
      shaderprogram = initprogram(vertexshader, fragmentshader) ; 
      islight = glGetUniformLocation(shaderprogram,"islight") ;        
      light0posn = glGetUniformLocation(shaderprogram,"light0posn") ;       
      light0color = glGetUniformLocation(shaderprogram,"light0color") ;       
      light1posn = glGetUniformLocation(shaderprogram,"light1posn") ;       
      light1color = glGetUniformLocation(shaderprogram,"light1color") ;       
      ambient = glGetUniformLocation(shaderprogram,"ambient") ;       
      diffuse = glGetUniformLocation(shaderprogram,"diffuse") ;       
      specular = glGetUniformLocation(shaderprogram,"specular") ;       
      shininess = glGetUniformLocation(shaderprogram,"shininess") ;       
}

void display() {
	glClearColor(0, 0, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glMatrixMode(GL_MODELVIEW);
	mat4 mv ; 
    const vec3 center(0.0,0.0,0.0) ; 

    if (useGlu) mv = glm::lookAt(eye,center,up) ; 
	else {
          mv = Transform::lookAt(eye,center,up) ; 
          mv = glm::transpose(mv) ; // accounting for row major
        }
    glLoadMatrixf(&mv[0][0]) ; 

        // Set Light and Material properties for the teapot
        // Lights are transformed by current modelview matrix. 
        // The shader can't do this globally. 
        // So we need to do so manually.  
        transformvec(light_position, light0) ; 
        transformvec(light_position1, light1) ; 
 
		glUniform4fv(light0posn, 1, light0) ; 
        glUniform4fv(light0color, 1, light_specular) ; 
        glUniform4fv(light1posn, 1, light1) ; 
        glUniform4fv(light1color, 1, light_specular1) ; 

        //glUniform4fv(ambient,1,small) ; 
        //glUniform4fv(diffuse,1,medium) ; 
	glUniform4fv(ambient,1,small) ; 
        glUniform4fv(diffuse,1,small) ; 
        glUniform4fv(specular,1,one) ; 
        glUniform1fv(shininess,1,high) ; 
        glUniform1i(islight,true) ;

        // Transformations for Teapot, involving translation and scaling 
        mat4 sc(1.0) , tr(1.0) ; 
        sc = Transform::scale(sx,sy,1.0) ; 
        tr = Transform::translate(tx,ty,0.0) ; 
        // Multiply the matrices, accounting for OpenGL and GLM.
        sc = glm::transpose(sc) ; tr = glm::transpose(tr) ; 
        mat4 transf  = mv * tr * sc ; // scale, then translate, then lookat.
        glLoadMatrixf(&transf[0][0]) ; 

	glutSolidTeapot(2);
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("HW1: Transformations");
	init();
	glutDisplayFunc(display);
	glutSpecialFunc(specialKey);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutReshapeWindow(600, 400);
	printHelp();
	glutMainLoop();
	return 0;
}
