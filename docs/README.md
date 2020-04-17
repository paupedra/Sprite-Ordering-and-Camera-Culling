# Camera Culling

In rendering, camera culling is a way of optimizing our Render calls so that only things visible inside the camera are drawn. This is very import for any kind of video game here is a list of reasons why you should care about culling:

There will be a bigger limit to the size of the map/levels in the game
There will be more processing power to be spent on gameplay
Game will run smoothly in machines with less resources
Highest resolution will be more easily achieved
It’s optimal

Now that you see why you would be interested in having culling in your game let’s go through some of the types of culling.

## Frustum Culling: 
This refers to the process of discarding any object completely outside the view of the camera. It effects performance dramatically and it’s mandatory for almost any kind of game. It is applicable to both 2D and 3D graphics.

### In 3D:
In 3D the usual approach involves creating two planes between the closest and furthest plane view of the camera, his creates the shape of a truncated pyramid the camera being the tip and the further plane the base. Then we should check for intersections between these two planes and the objects in the scene and discard the ones which are completely outside the view.
