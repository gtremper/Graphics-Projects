Graham Tremper: cs184-bj: http://inst.cs.berkeley.edu/~cs184-bj/
Wesley Prager:  cs184-dy: http://inst.cs.berkeley.edu/~cs184-dy/

For this project, we extended our raytracer. The features we added include:
	-a KD-tree acceleration structure
	-antialiasing
		-uses Monte Carlo sampling
	-refraction
	-area lighting for soft shadows
		-for this, we made a sphere around a point light and sampled from random points within the sphere

To render a scene, simply call "raytracer" with the scene file as the argument.