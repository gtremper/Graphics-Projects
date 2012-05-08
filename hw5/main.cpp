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

	Intersection hit = scene.KDTree->intersect(ray);

	//Intersection hit = Intersection(scene.objects, ray);
	
	
	if(!hit.primative) {
		return vec3(0,0,0); //background color
	}
	
	vec3 color = hit.primative->ambient;
	color += hit.primative->emission;
	vec3 normal = hit.primative->getNormal(hit.point);
	double c1 = -glm::dot(normal, ray.direction);
	
	vector<Light*>::iterator light=scene.lights.begin();
	for(; light!=scene.lights.end(); ++light){
		color += (*light)->shade(hit,*scene.KDTree,normal);
	}
	
	Ray reflectedRay = Ray(hit.point+EPSILON*normal, ray.direction+(2.*normal*c1));
	
	if(depth != 1) {
		color += hit.primative->specular * findColor(scene, reflectedRay, depth-1);
		if(hit.primative->refractivity) {
			
			double n = 1.0/hit.primative->indexofrefraction; // first number is the refractive index of air
			double c2 = sqrt(1 - n*n * (1 - c1*c1));
			Ray refractedRay = Ray(hit.point, -glm::normalize((n*ray.direction) + (n*c1-c2)*normal));
			if(c1>0.0){
				refractedRay.origin -= EPSILON*normal;
			} else {
				refractedRay.origin += EPSILON*normal;
			}
			color += hit.primative->refractivity * findColor(scene, refractedRay, depth-1);
		}
	}
	return color;
}

void raytrace(Scene& scene) {
	FreeImage_Initialise();
	
	FIBITMAP* bitmap = FreeImage_Allocate(scene.width, scene.height, BPP);
	
	if (!bitmap) exit(1);
	double subdivisions = 3;
	double subdivide = 1/subdivisions;
	
	#pragma omp parallel for
	for (int j=0; j<scene.height; j++){
		int tid = omp_get_thread_num();
		if(tid == 0) {
		   clog << "Progress: "<< (j*100*omp_get_num_threads())/scene.height <<"%"<<"\r";
		}
		RGBQUAD rgb;
		for (int i=0; i<scene.width; i++) {
			vec3 color;
			for(double a=0; a<subdivisions; a+=1) {
				for(double b=0; b<subdivisions; b+=1) {
					double randomNum1 = ((double)rand()/(double)RAND_MAX) * subdivide;
					double randomNum2 = ((double)rand()/(double)RAND_MAX) * subdivide;
					Ray ray = scene.castEyeRay(i+(a*subdivide) + randomNum1,j+(b*subdivide)+randomNum2);
					color += findColor(scene, ray, scene.maxdepth);
				}
			}
			color /= (subdivisions * subdivisions);
			
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