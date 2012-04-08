Graham Tremper: cs184-bj: http://inst.cs.berkeley.edu/~cs184-bj/
Wesley Prager:  cs184-dy: http://inst.cs.berkeley.edu/~cs184-dy/

Right now, we can mostly parse the scene file, set up the coordinate frame for the camera, send out rays from the camera, transform the shapes, and calculate the intersections for triangles and spheres. As of right now, the shading just uses the ambient color.

Next week, we will work on
	-calculate normals for spheres and triangles
	-lighting
		-shadows
		-shading (Phong)
		-implement recursive ray tracing for reflection
	-implement KD-tree for acceleration structure

We will work on these things roughly in this order.