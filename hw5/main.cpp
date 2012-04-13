/* This is the main file for the raytracer */

#include <iostream>
#include <cstdio>
#include <stack>
#include <string>
#include <vector>

#include "FreeImage.h"
#include "Shapes.h"
#include "Scene.h"
#include "Intersection.h"

#define BPP 24

using namespace std;

vec3 findColor(Scene& scene, Ray& ray, int depth) {
	Intersection hit = Intersection(scene.objects, ray);
	if (!hit.primative) {
		return vec3(0,0,0); //background color
	}
	vec3 normal = hit.primative->getNormal(hit.point);
	
	vec3 color = hit.primative->ambient;
	
	/*
	color += hit.primative->emmision;
	vector<Light>::iterator light=scene.lights.begin();
	for(; light!=scene.lights.end(); ++light){
		if ( light->isVisible(hit.point, scene.objects) ){
			color += // color shit
		}
	}
	*/
	return color;
}

void raytrace(Scene& scene) {
	FreeImage_Initialise();
	
	FIBITMAP* bitmap = FreeImage_Allocate(scene.width, scene.height, BPP);
	RGBQUAD rgb;
	Ray ray;
	
	if (!bitmap) exit(1);
	
	for (int j=0; j<scene.height; j++){
		printf("Progress: %d\r",j*100/scene.height);
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
	Scene scene(argv[1]);
	raytrace(scene);
	return 0;
}