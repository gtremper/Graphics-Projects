Scene Viewer
============

The first thing we did was complete hw1's Transform.cpp by adding the perspective matrix as well as translation and scale methods.

In order to properly draw the scene, we needed to create a data structure that would store the commands and arguments specified in the input file. We created a struct to store this information and made a vector of them to store the series of commands in the correct order. 

Next, we added the parser to take in the txt file and set the global variables to their initial values. It created a list of commands that was used by the function drawObjects() to render the scene.

Next, we extended the lights from hw1 to handle more than two lights by storing the light positions and light colors in arrays. We also handled the case when there is a directional light. Then, we added the 0-9 keys to be able to control each light. This required creating an array of up vectors for each light. These were specified whenever a user selects a light to manipulate. We also created a separate light_position_init array to store the initial location of the lights, which we used for "reset."

Additional Features:
* Set default parameters to ensure the program can handle any input.
* You can turn on and off the lights using the 'l' key.
