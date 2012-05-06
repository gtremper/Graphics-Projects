/* This is the main file for the raytracer */

#include <iostream>
#include <cstdio>
#include <stack>
#include <string>
#include <vector>
#include "omp.h"
#include <time.h>

#include "FreeImage.h"
#include "Shapes.h"
#include "Intersection.h"
#include "Light.h"
#include "Scene.h"

#define BPP 24
#define EPSILON 0.0000001

using namespace std;

vec3 findColor(Scene& scene, Ray& ray, int depth) {
	//Intersection hit = scene.KDTree->intersect(ray);
	
	Intersection hit = Intersection(scene.objects, ray);
	if (!hit.primative) {
		return vec3(0,0,0); //background color
	}
	
	vec3 color = hit.primative->ambient;
	color += hit.primative->emission;
	
	vec3 normal = hit.primative->getNormal(hit.point);
	
	vector<Light*>::iterator light=scene.lights.begin();
	for(; light!=scene.lights.end(); ++light){
		color += (*light)->shade(hit,scene.objects,normal);
	}
	
	Ray reflectedRay = Ray(hit.point+EPSILON*normal, ray.direction-(2.*normal*glm::dot(normal, ray.direction)));
	
	
	if (depth != 1){
		color += (hit.primative->specular*findColor(scene, reflectedRay, --depth));
	}
	return color;
}

void raytrace(Scene& scene) {
	FreeImage_Initialise();
	
	FIBITMAP* bitmap = FreeImage_Allocate(scene.width, scene.height, BPP);
	
	if (!bitmap) exit(1);
	double subdivisions = .25;
	
	#pragma omp parallel for
	for (int j=0; j<scene.height; j++){
		int tid = omp_get_thread_num();
		if(tid == 0) {
		   clog << "Progress: "<< (j*100*omp_get_num_threads())/scene.height <<"%"<<"\r";
		}
		RGBQUAD rgb;
		for (int i=0; i<scene.width; i++) {
			vec3 color;
			for(double a=i; a<i+1; a+=subdivisions) {
				double randomNum1 = ((double)rand()/(double)RAND_MAX) * subdivisions;
				for(double b=j; b<j+1; b+=subdivisions) {
					double randomNum2 = ((double)rand()/(double)RAND_MAX) * subdivisions;
					Ray ray = scene.castEyeRay(a + randomNum1,b + randomNum2);
					color += findColor(scene, ray, scene.maxdepth);
				}
			}
			
			// Ray ray1 = scene.castEyeRay(i+.125,j+.125);
			// Ray ray2 = scene.castEyeRay(i+.125,j+.375);
			// Ray ray3 = scene.castEyeRay(i+.125,j+.625);
			// Ray ray4 = scene.castEyeRay(i+.125,j+.875);
			// Ray ray5 = scene.castEyeRay(i+.375,j+.125);
			// Ray ray6 = scene.castEyeRay(i+.375,j+.375);
			// Ray ray7 = scene.castEyeRay(i+.375,j+.625);
			// Ray ray8 = scene.castEyeRay(i+.375,j+.875);
			// Ray ray9 = scene.castEyeRay(i+.625,j+.125);
			// Ray ray10 = scene.castEyeRay(i+.625,j+.375);
			// Ray ray11 = scene.castEyeRay(i+.625,j+.625);
			// Ray ray12 = scene.castEyeRay(i+.625,j+.875);
			// Ray ray13 = scene.castEyeRay(i+.875,j+.125);
			// Ray ray14 = scene.castEyeRay(i+.875,j+.375);
			// Ray ray15 = scene.castEyeRay(i+.875,j+.625);
			// Ray ray16 = scene.castEyeRay(i+.875,j+.875);
			// 
			// vec3 color = findColor(scene,ray1,scene.maxdepth);
			// color += findColor(scene,ray2,scene.maxdepth);
			// color += findColor(scene,ray3,scene.maxdepth);
			// color += findColor(scene,ray4,scene.maxdepth);
			// color += findColor(scene,ray5,scene.maxdepth);
			// color += findColor(scene,ray6,scene.maxdepth);
			// color += findColor(scene,ray7,scene.maxdepth);
			// color += findColor(scene,ray8,scene.maxdepth);
			// color += findColor(scene,ray9,scene.maxdepth);
			// color += findColor(scene,ray10,scene.maxdepth);
			// color += findColor(scene,ray11,scene.maxdepth);
			// color += findColor(scene,ray12,scene.maxdepth);
			// color += findColor(scene,ray13,scene.maxdepth);
			// color += findColor(scene,ray14,scene.maxdepth);
			// color += findColor(scene,ray15,scene.maxdepth);
			// color += findColor(scene,ray16,scene.maxdepth);
			color /= 16;
			
			rgb.rgbRed = min(color[0],1.0)*255.0;
			rgb.rgbGreen = min(color[1],1.0)*255.0;
			rgb.rgbBlue = min(color[2],1.0)*255.0;
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
	srand(time(0));
	raytrace(scene);
	return 0;
}