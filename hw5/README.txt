Graham Tremper: cs184-bj: http://inst.cs.berkeley.edu/~cs184-bj/
Wesley Prager:  cs184-dy: http://inst.cs.berkeley.edu/~cs184-dy/

To render a scene, simply call "raytracer" with the scene file as the argument

Our ray-tracer correctly implements all the basic requirements for HW5 except for the acceleration structure. We added some additional features that improve the amount of control over the scene and performance. These include:

-each light can have its own attenuation parameters
-basic parallelism in main raytracing loop using OpenMP