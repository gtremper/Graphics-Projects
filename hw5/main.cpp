/* This is the main file for the raytracer */

#include <iostream>
#include <stack>
#include <string>

#include "FreeImage.h"
#include "Shapes.h"
#include "Scene.h"
#include "Intersection.h"

#define BPP 24

using namespace std;

vec3 findColor(Scene& scene, Ray& ray, int depth) {
	//cout << "find color" << endl;
	Intersection hit = Intersection(scene.objects, ray);
	if (!hit.primative) {
		//cout << "!hit" << endl; 
		return vec3(0,0.0,0.1);
	}
	
	return vec3(1,0,0);
}

void raytrace(Scene& scene) {
	FreeImage_Initialise();
	
	FIBITMAP* bitmap = FreeImage_Allocate(scene.width, scene.height, BPP);
	RGBQUAD rgb;
	
	if (!bitmap) exit(1);
	
	// Draw function. This draws gradient blue to green
	Ray ray;
	for (int j=0; j<scene.height; j++){
		//if (i%20==0) cout << "i equals " << i << endl;
		for (int i=0; i<scene.width; i++) {
			scene.castEyeRay(i,j,ray);
			vec3 color = findColor(scene,ray,scene.maxdepth);
			rgb.rgbRed = color[0]*255.0;
			rgb.rgbGreen = color[1]*255.0;
			rgb.rgbBlue = color[2]*255.0;
			FreeImage_SetPixelColor(bitmap,i,j,&rgb);
		}
	}
	
	if (FreeImage_Save(FIF_PNG, bitmap, scene.filename.c_str(), 0)){
		cout << "Image successfully saved!" << endl;
	}
	
	FreeImage_DeInitialise();
}


int main(int argc, char* argv[]){
	if(argc != 2) {
		cerr << "You need 1 scene file as the argument" << endl;
		exit(1);
	}
	Scene scene;
	scene.parse(argv[1]);
	raytrace(scene);
	return 0;
}
	
	
	
	/*
	FreeImage_Initialise();
	FIBITMAP* bitmap = FreeImage_Allocate(WIDTH, HEIGHT, BPP);
	RGBQUAD color;
	
	if (!bitmap) exit(1);
	
	// Draw function. This draws gradient blue to green
	for (int i=0; i<WIDTH; i++){
		for (int j=0; j<HEIGHT; j++) {
			color.rgbRed = 0;
			color.rgbGreen = (double)i / WIDTH * 255.0;
			color.rgbBlue = (double)j / WIDTH * 255.0;
			FreeImage_SetPixelColor(bitmap,i,j,&color);
		}
	}
	
	if (FreeImage_Save(FIF_PNG, bitmap, "test.png", 0)){
		cout << "Image successfully saved!" << endl;
	}
	
	FreeImage_DeInitialise();
}
*/