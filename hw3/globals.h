/* Globals needed by the parser and drawer */

#ifndef GLOBALS_H
#define GLOBALS_H

extern vec3 eyeinit;
extern float pitchInit;
extern float yawInit;
extern int width, height;
extern float fovy; 
extern int numLights;
extern const int MAXLIGHTS;
extern vec4 light_position[];
extern vec4 light_specular[];
extern bool useLights;
extern GLuint shaderprogram;
extern GLuint istex;
extern bool wire;

#endif