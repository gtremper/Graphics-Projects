The first thing we did was add to hw1's Transform.cpp by adding the perspective matrix as well as translation and scale methods.

Next, we extended the lights from hw1 to handle more than two lights by storing the light positions and light color in arrays. We also handled the case when there is a directional light. Then, we added the 0-9 keys to be able to control each light. We also created a separate light_position_init array to store the initial location of the lights, which we used for "reset."

For the main part, we used a "command" data structure. We made a vector to store the commands that change for every object, and looped through them to set the conditions for the particular object.

Notes:
	added a default scene so it doesn't break if no size/camera commands are given
	added a way to turn off lights