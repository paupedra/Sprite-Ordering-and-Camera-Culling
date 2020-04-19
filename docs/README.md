# Camera Culling

In rendering, camera culling is a way of optimizing our Draw calls so that only things visible inside the camera view are drawn. This is very import for any kind of video game so here is a list of reasons why you should care about culling:

+ There will be a bigger limit to the size of the map/levels in the game
+ There will be more processing power to be spent on gameplay
+ Game will run smoothly in machines with less resources
+ Highest resolution and LOD (Level Of Detail) will be more easily achieved
+ It’s optimal!

Now that you see why you would be interested in having culling in your game let’s go through some of the types of culling.

## 1. Frustum Culling: 
This refers to the process of discarding any object completely outside the view of the camera. It effects performance dramatically and it’s mandatory for almost any kind of game. It is applicable to both 2D and 3D graphics.

### In 3D:
In 3D the usual approach involves creating two planes between the closest and furthest plane view of the camera, his creates the shape of a truncated pyramid representing the area which will be viewed by the camera, the camera being the tip and the further plane the base. Then we should check for intersections between these two planes and the objects in the scene and discard the ones which are completely outside the view.

![frustum culling](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/frustum_culling_1.png "Frustum CUlling Pyramid")

Image found [here](https://murlengine.com/tutorials/en/tut0101_cube.php) portraying the two planes and the area which should be rendered. Also [here’s](https://www.youtube.com/watch?v=4p-E_31XOPM) a very good video briefly explaining how it is calculated:

![Zero dawn showing frustum culling](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/zero_dawn_gif.gif)

Here we have a visual representation on how this works insinde Horizon Zero Dawn.

### In 2D:
In 2D frustum is a bit simpler since there is one less dimension the only calculations needed are to check if an object is inside our camera rectangle. But this doesn’t mean that it is easy to do this efficiently. In

## 2. Backface Cull:
This is a 3D exclusive culling which takes the polygons from single objects and calculates if they will be visible to the camera, and if they are not these won’t be rendered. Let’s say we are seeing a cube from the front, the only polygon which will be seen by the camera is a single face, so the backface cull will make sure the other squares are not drawn.

![backface_cull](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/backface_culling_1.PNG "Backface Cull calculus example")

Image taken from [Here](https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/3d-backface-culling-r1088/), it's also a great example on how this could be implemented.

## 3. Occlusion culling: 
This type of culling makes sure that objects which are hidden/occluded behind other objects are not drawn. 

There is an image taken from [here](https://www.gamasutra.com/view/feature/131801/occlusion_culling_algorithms.php) shows a case in which occlusion can be very useful. The article is pretty interesting, it explains with more detail the occlusion process so I recommend checking it out.

### In 3D
To be as efficient as possible we want to only draw the pixels which will be visible and not the ones occluded. Even the pixels from polygons intersecting between them.

#### Z-Buffering or depth buffering:
Z-buffering is the management of depth coordinates in 3D graphics rendering. It is used to solve the problem of visibility between objects which are aligned between them with the camera. Z-value refers to the measure of the perpendicular line between the 3D space coordinates of a pixel on the projection plane and the camera. Every single pixel in the screen has a Z-value and in each frame the objects which intersect with the pixel's line are looped and the distance between that point of intersection and the camera is saved in the Z-buffer. Every object modifies the Z-value if it's Z is lowest than the previous value so in the end only the closest object will be drawn.

![Z-Buffering](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/Z-Buffering_1.PNG "Z-Buffering algorithm simplification")

Image taken from [here](https://larranaga.github.io/Blog/). [Here](https://www.youtube.com/watch?v=yhwg_O5HBwQ) is a very simple explanation on how it’s usually done. Also [Here](https://www.youtube.com/watch?v=HyVc0X9JKpg) is another great tutorial.

### In 2D
There is no popular solution to deal with occlusion culling in 2D that I know of, though I have seen some implementations around the internet, they vary a lot depending on the game. For example on our tiled game, we could use occlusion when we have a building, we will not need to draw the tiles it covers but this is hardly applicable to any other kind of game, so I encourage you to find in what ways you can optimize your 2D game with Occlusion culling!

This should help as a visual summary on how frustum and occlusion culling combined should look like in a 3D graphics game. I will also leave a link to the website this was found in which contains a lot of information on how to achieve faster rendering. It talks extensively about the points I brought up and even lower level graphics theory. [High-Level Strategic Tools for Fast Rendering](https://techpubs.jurassic.nl/manuals/nt/developer/Optimizer_PG/sgi_html/pt02.html)

![Frustum and Occlusion representation](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/perfect_example.png "Frustum and Occlusion representation")

# Sprite ordering


## Two dimensions:
Sprite rendering order is a necessary feature in a video game, without it the game would lack the feeling of depth and credibility from the player since in real life objects overlap each other all the time depending on their distance to the viewer.

In orthogonal view only two dimensions are represented. Some examples of this kind of view used in games would be side scrolling platformers or top down games. This means that the only depth that will be represented is between the objects which are further or closer to the camera in a fixed way. In this kind of view layers are used to define in which order sprites are drawn.

![metroid_example](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/metroid_example.png)

The background should always be drawn under the rest of the objects and the rest can be sorted in any way since they usually won’t collide, though this can change depending on the game, an usual order would be: background - platforms/floor - entities. As an example a platformer would have a leyer for: Platforms, player character, enemies and background.

## Three dimensions
But in our case we have done the jump to what is usually called 2.5D or 3/4 which means that the elements are no longer in a fixed order and we can now “view” 3 dimensions. Though this third dimension is not really there because we are using 2D sprites we still have to create the illusion that it exists. This demands a way of dynamically change the order in which objects are drawn to remain consistent in showing depth when we have moving entities or creating new ones.

![Final Fantasy Tactics Example](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/final_fantasy_tactics_example.png "Final Fantasy Tactics Example")

Objects which are further from the camera are the ones drawn first therefore will be overlapped by closer objects so we should order them using their Y position on screen from top to bottom. The position used to make this calculations should usually be their central position on the map. Now I would like to go through some implementations done by fellow developers and some of the problems and solutions they found.

## Sprite Ordering methods found:

### Sprite cutting and layers:

This methods consists on dividing the static sprites in two different parts, the lower or base which will be overlapped by the dynamic entities moving close to it and the top part which will overlap them.

We can see this in action in “The Legend of Zelda: A link to the past” for the SNES with this tree here, though it is also used in other structures such as houses.

![zelda_example_1](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/zelda_example_1.png "zelda_example_1")

As I hope you can appreciate Link is under the Tree Top and at the same time over the Trunk of the tree so we can see that these are clearly two different sprites layered in a certain order. This next image portrays exactly how they are ordered.

![zelda_example_2](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/zelda_example_2.png "zelda_example_2")

Also in the following picture we can see how even the dynamic entities are not ordered. This is probably because they couldn't afford to use so many resources on doing dynamic ordering. Here is an image to prove this.

![zelda_example_3](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/zelda_example_3.png "zelda_example_3")

Another example of this that I found in an isometric game is from the game Pocket City for Android and IOS. This is a very rudimentary approach which works in this game but is not really applicable to any isometric game. It also takes extra work to set up for every single different sprite in the game so it is not very sustainable. Here are some visuals to show how they did it.

![pocket_city_2](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/pocket_city_2.png)

![pocket_city_1](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/pocket_city_1.gif)

### Dynamic ordering using Y position

This is the dsired method,and I would argue it is the most used in 2D non orthogonal views. All entities will havea point in the center of their base, and every frame they will be sorted from highest to lowest Y position so that they are drawn in the correct order.

I will explain how I implemented this later on.

# My Implementation:

First problem we have to deal with is camera culling. To avoid using Brute Force I'm going tu use space partitioning. This is a technique which will dramatically help with being able to find objects which are in certain positions without having to loop through all of the Tiles or Entities. to do this I will introduce you to Quadtrees.

## Quadtrees:

A Quadtree is a tree data structure with each node having 4 children. They are commonly used to partition space in a 2D space. These are commonly used in image processing, mesh generation, view frustum culling of terrain data and efficient collision detection amongst others.

Maybe you noticed that I just mentioned two very important things, view frustum culling and efficient collision detection. Collision detection is not part of this research but it’s a very important and significant optimization in every game which could use any kind of collision detection, maybe even for looking for targets to attack for an RTS game… Anyways I’ll leave a very good series of videos [here](https://www.youtube.com/watch?v=z0YFFg_nBjw&t=799s) which shows how to create a quadtree and then use it for optimizing collision detection.

But let’s get into the important use here, frustum culling. First I’d like to talk a bit more about how quadtrees work so that you can later try and implement this.

![Quadtree space partition](https://raw.githubusercontent.com/paupedra/Sprite-Ordering-and-Camera-Culling/master/docs/images/Quadtrees_1.png "QUadtree space partition")

As any tree data structures quadtrees use recursion to be able to call their subsequent children. A space partitioning quadtree is created by inserting points or objects into it, first it will check if the object is inside the boundaries of the tree, if so, if the node is at its full capacity it will divide it into four and do the same process again for every one of the four new nodes. This will happen until the quadtree reaches the maximum levels which are defined when it’s created.

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
