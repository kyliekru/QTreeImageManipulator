# QTree Image Manipulator

This project is a C++ program designed to manipulate images using a quadtree (`QTree`) data structure. It provides functionalities to build a quadtree from an image, render the image from the quadtree, flip the image horizontally, rotate the image counter-clockwise, and prune the quadtree to simplify the image based on a tolerance value.

## Features

- **Quadtree Construction**: Build a quadtree from an image for efficient storage and manipulation.
- **Rendering**: Render an image from the quadtree at various scales.
- **Horizontal Flipping**: Flip the image horizontally using the quadtree structure.
- **Counter-Clockwise Rotation**: Rotate the image counter-clockwise using the quadtree structure.
- **Pruning**: Prune the quadtree to simplify the image based on a specified tolerance, reducing the number of nodes and leaves.

## Usage  

**Build**: ```make```  
**Run**: ```./pa3```


## Dependencies

- Standard C++ libraries
- PNG image handling via `cs221util::PNG`
