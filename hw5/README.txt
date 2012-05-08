Graham Tremper: cs184-bj: http://inst.cs.berkeley.edu/~cs184-bj/
Wesley Prager:  cs184-dy: http://inst.cs.berkeley.edu/~cs184-dy/

For this project, we extended our raytracer. The features we added include:
	-a KD-tree acceleration structure
	-antialiasing
		-uses Monte Carlo sampling
	-refraction
	-area lighting for soft shadows
		-for this, we made a sphere around a point light and sampled from random points within the sphere

The added commands for the scene files are:
	-antialias (for the number of times to subdivide the pixel; e.g. "4" divides the pixel into a 4x4 grid)
	-indexofrefraction
	-refractivity
	-shadowrays (for the number of rays to shoot at random points in the area light)
	-lightradius (for the size of the area light)

To render a scene, simply call "raytracer" with the scene file as the argument.

The bug we still have is with the KD-tree. In the tree traversal, some objects do not get added to a subdivision making that object, or parts of that object, disappear. It only happens occasionally though. We know that it is a floating point precision error because it only occurs on edge cases when the object lies on both sides of the subdivision. We know that the object is in the tree because it appears when rendering from different angles, or splitting down different axes.