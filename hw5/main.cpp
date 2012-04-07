/* This is the main file for the raytracer */

#include <iostream>
#include <cstdio>
#include <stack>
#include <string>
#include <time.h>

#include "FreeImage.h"
#include "Shapes.h"
#include "Scene.h"
#include "Intersection.h"

#define BPP 24

using namespace std;

vec3 findColor(Scene& scene, Ray& ray, int depth) {
	Intersection hit = Intersection(scene.objects, ray);
	if (!hit.primative) {
		return vec3(0,0,0);
	}
	return hit.primative->ambient;
}

void raytrace(Scene& scene) {
	FreeImage_Initialise();
	
	FIBITMAP* bitmap = FreeImage_Allocate(scene.width, scene.height, BPP);
	RGBQUAD rgb;
	Ray ray;
	
	if (!bitmap) exit(1);
	
	for (int j=0; j<scene.height; j++){
		printf("Progress: %2.0f%%\r",(float)j/scene.height);
		for (int i=0; i<scene.width; i++) {
			scene.castEyeRay(i,j,ray);
			vec3 color = findColor(scene,ray,scene.maxdepth);
			rgb.rgbRed = color[0]*255.0;
			rgb.rgbGreen = color[1]*255.0;
			rgb.rgbBlue = color[2]*255.0;
			FreeImage_SetPixelColor(bitmap,i,j,&rgb);
		}
	}
	printf("Progress: 100%%\n");
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
	time_t start,end;
	time(&start);
	
	Scene scene;
	scene.parse(argv[1]);
	raytrace(scene);
	
	time(&end);
	double dif = difftime(end,start);
	printf("Render time: %.3f seconds\n",dif);
	return 0;
}