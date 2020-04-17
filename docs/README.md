Camera Culling

In rendering, camera culling is a way of optimizing our Render calls so that only things visible inside the camera are drawn. This is very import for any kind of video game here is a list of reasons why you should care about culling:

There will be a bigger limit to the size of the map/levels in the game
There will be more processing power to be spent on gameplay
Game will run smoothly in machines with less resources
Highest resolution will be more easily achieved
It’s optimal

Now that you see why you would be interested in having culling in your game let’s go through some of the types of culling.

-Frustum Culling: 
This refers to the process of discarding any object completely outside the view of the camera. It effects performance dramatically and it’s mandatory for almost any kind of game. It is applicable to both 2D and 3D.

3D:
In 3D the usual approach involves creating two planes between the closest and furthest plane view of the camera, his creates the shape of a truncated pyramid the camera being the tip and the further plane the base. Then we should check for intersections between these two planes and the objects in the scene and discard the ones which are completely outside the view.

Here is an image I found [here](https://murlengine.com/tutorials/en/tut0101_cube.php) portraying the two planes and the area which should be rendered. And [here’s](https://www.youtube.com/watch?v=4p-E_31XOPM) a very good video briefly explaining how it is calculated:

2D:
In 2D frustum is a bit simpler since there is one less dimension the only calculations needed are to check if an object is inside our camera rectangle. But this doesn’t mean that it is easy to do this efficiently, we will see later how this can be done in our game.


-Occlusion culling: 
This type of culling makes sure that objects which are hidden/occluded behind other objects are not drawn. 

This image taken from [here](https://www.gamasutra.com/view/feature/131801/occlusion_culling_algorithms.php) shows a case in which occlusion can be very useful.

3D
-Z-Buffering or depth buffering:
Z-buffering is the management of depth coordinates in 3D graphics rendering. It is used to solve the problem of visibility between objects which are aligned between them with the camera. Z-value refers to the measure of the perpendicular line between the 3D space coordinates of a pixel on the projection plane and the camera.

To determine what pixel to draw the z-value of every shape is calculated. The pixels with the lowest value are drawn at the end.

[Here](https://www.youtube.com/watch?v=yhwg_O5HBwQ) is a very simple explanation on how it’s usually done. Also [Here](https://www.youtube.com/watch?v=HyVc0X9JKpg) is another great tutorial.

2D
There is no popular solution to deal with occlusion culling in 2D that I know of, though I have seen some implementations around the internet, they vary a lot depending on the game. For example on our tiled game, we could use occlusion when we have a building, we will not need to draw the tiles it covers.

Backface Cull:
This is a 3D exclusive culling which takes the polygons from single objects and calculates if they will be visible to the camera, and if they are not these won’t be rendered. Let’s say we are seeing a cube from the front, the only polygon which will be seen by the camera is a single face, so the backface cull will make sure the other squares are not drawn.

[Here](https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/3d-backface-culling-r1088/) is a good example on how this could be implemented.



