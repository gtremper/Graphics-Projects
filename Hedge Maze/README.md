# Hedge Maze Scene

To run the scene after compiling, run "./transforms HedgeMaze.txt models.txt" from the command line.

The movement is similar to a first person game where moving the mouse controls where to look, and the "WASD" keys control movement. You can move the mouse without moving the eye by clicking first and then dragging. By pressing the "f" key, you can switch to fly mode where you can leave the ground and get a bird's eye view of the scene. In this mode, you can move vertically in addition to along the horizontal plane. The orientation we are using is such that the two entrances to the maze are at the top left corner (facing up), and bottom right corner (facing right).

The object created by hand is all the walls of the maze itself. This can pretty much be seen from anywhere.

The objects loaded from an .obj file are the maze walls, the ground, and the cube. The first two can be seen anywhere, while the cube is in the lower right corner of the maze.

The object created from a .raw triangle file is the 3D triforce symbol, which can be found near the bottom left corner of the maze.

The two objects that are textured are the maze walls and the ground.

One object that is shiny in appearance is the large teapot found in the center of the maze. The smaller teapot right next to that is dull in appearance.

There is one overhead directional white light.

There are five point lights of various colors located in the four corners and the center of the scene.

The two teapots in the center of the maze is the same object that is instantiated twice. They are of different size, translated differently, and have different specular properties.

The maze walls can switch to and from wireframe mode. This is toggled using the 'q' key.

The maze walls, the ground, all elements of the table, the teapots, and the sphere all have correct normals. The table can be found in the central area of the maze, while the sphere is located near the top right corner.

The 'l' key toggles both lights and textures, while the 't' key just toggles textures and leaves the lights on.

In the vertex shader, we added light attenuation, and added a cartoon-like shader. This can be toggled with the 'c' key.

We calculated the smooth Lambertian shading at either the vertex or fragment level. This can be toggled with the 'v' key.

The larger teapot is constantly animated, while the smaller teapot's animation can be toggled using the 'p' key.

Graham Tremper
http://inst.cs.berkeley.edu/~cs184-bj/

Wesley Prager
http://inst.cs.berkeley.edu/~cs184-dy/
