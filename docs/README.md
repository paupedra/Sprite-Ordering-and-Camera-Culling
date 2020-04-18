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

![Frustum_Culling_1](https://github.com/paupedra/Sprite-Ordering-and-Camera-Culling/blob/master/docs/images/frustum_culling_1.png"Is this working?")

Here is an image I found [here](https://murlengine.com/tutorials/en/tut0101_cube.php) portraying the two planes and the area which should be rendered. Also [here’s](https://www.youtube.com/watch?v=4p-E_31XOPM) a very good video briefly explaining how it is calculated:

### In 2D:
In 2D frustum is a bit simpler since there is one less dimension the only calculations needed are to check if an object is inside our camera rectangle. But this doesn’t mean that it is easy to do this efficiently, we will see later how this can be done in our game.


## Occlusion culling: 
This type of culling makes sure that objects which are hidden/occluded behind other objects are not drawn. 


There is an image taken from [here](https://www.gamasutra.com/view/feature/131801/occlusion_culling_algorithms.php) shows a case in which occlusion can be very useful. The article is pretty interesting, it explains with more detail the occlusion process so I recommend checking it out.

### 3D

#### Z-Buffering or depth buffering:
Z-buffering is the management of depth coordinates in 3D graphics rendering. It is used to solve the problem of visibility between objects which are aligned between them with the camera. Z-value refers to the measure of the perpendicular line between the 3D space coordinates of a pixel on the projection plane and the camera. Every single pixel in the screen has a Z-value and in each frame the objects which intersect with the pixel's line are looped and the distance between that point of intersection and the camera is saved in the Z-buffer. Every object modifies the Z-value if it's Z is lowest than the previous value so in the end only the closest object will be drawn.



![]()
Image taken from [here](https://larranaga.github.io/Blog/). [Here](https://www.youtube.com/watch?v=yhwg_O5HBwQ) is a very simple explanation on how it’s usually done. Also [Here](https://www.youtube.com/watch?v=HyVc0X9JKpg) is another great tutorial.

### 2D
There is no popular solution to deal with occlusion culling in 2D that I know of, though I have seen some implementations around the internet, they vary a lot depending on the game. For example on our tiled game, we could use occlusion when we have a building, we will not need to draw the tiles it covers but this is hardly applicable to any other kind of game.

## Backface Cull:
This is a 3D exclusive culling which takes the polygons from single objects and calculates if they will be visible to the camera, and if they are not these won’t be rendered. Let’s say we are seeing a cube from the front, the only polygon which will be seen by the camera is a single face, so the backface cull will make sure the other squares are not drawn.

[Here](https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/3d-backface-culling-r1088/) is a good example on how this could be implemented.





# Links:

I'll leave here all the links I used for my research as well other articles and websites which I thought had very good information if someone wants to really expand their knowladge in these subjects, hope they are useful to you!

[Sorting for isometric Moving platforms] (https://gamedevelopment.tutsplus.com/tutorials/isometric-depth-sorting-for-moving-platforms--cms-30226)

[Advcanced Isometric Sprite Sorting Tutorial] (https://www.youtube.com/watch?v=yRZlVrinw9I)

[What I learned from trying to make an Isometric game in Unity](https://www.gamasutra.com/blogs/MartinPane/20170309/290889/What_I_learned_from_trying_to_make_an_Isometric_game_in_Unity.php)


Culling:
Frustum:
[View Frustum Culling Lighthouse3d](http://www.lighthouse3d.com/tutorials/view-frustum-culling/)
[Math for Game Developers - Frustum Culling](https://www.youtube.com/watch?v=4p-E_31XOPM)
[Frustum Culling Gamedev](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/)

occlusion:
[Z-Buffering](https://larranaga.github.io/Blog/)
[](https://www.gamasutra.com/view/feature/131801/occlusion_culling_algorithms.php)
[](https://en.wikipedia.org/wiki/Painter%27s_algorithm)
[](https://en.wikipedia.org/wiki/Z-buffering)
[](https://www.gamesindustry.biz/articles/2016-12-07-overview-on-popular-occlusion-culling-techniques)

Backface Cull:
[](https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/3d-backface-culling-r1088/)


[](https://sherzock.github.io/Sprite-Ordering-and-Camera-Culling/)
[](https://christt105.github.io/Sprite_Ordering_and_Camera_Culling_Personal_Research/)
[](https://ferba93.github.io/Camera-Culling-and-Sprite-Ordering/)
[](https://gamedev.stackexchange.com/questions/25896/how-do-i-find-which-isometric-tiles-are-inside-the-cameras-current-view)
[](https://forum.yoyogames.com/index.php?threads/smart-isometric-drawing-order.57356/)
[](https://en.wikipedia.org/wiki/Video_game_graphics#Top-down_perspective)
