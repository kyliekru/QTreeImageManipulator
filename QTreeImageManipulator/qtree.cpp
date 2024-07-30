/**
 * @file qtree.cpp
 * @description student implementation of QTree class used for storing image data
 *              CPSC 221 PA3
 *
 *              SUBMIT THIS FILE
 */

#include "qtree.h"

/**
 * Constructor that builds a QTree out of the given PNG.
 * Every leaf in the tree corresponds to a pixel in the PNG.
 * Every non-leaf node corresponds to a rectangle of pixels
 * in the original PNG, represented by an (x,y) pair for the
 * upper left corner of the rectangle and an (x,y) pair for
 * lower right corner of the rectangle. In addition, the Node
 * stores a pixel representing the average color over the
 * rectangle.
 *
 * The average color for each node in your implementation MUST
 * be determined in constant time. HINT: this will lead to nodes
 * at shallower levels of the tree to accumulate some error in their
 * average color value, but we will accept this consequence in
 * exchange for faster tree construction.
 * Note that we will be looking for specific color values in our
 * autograder, so if you instead perform a slow but accurate
 * average color computation, you will likely fail the test cases!
 *
 * Every node's children correspond to a partition of the
 * node's rectangle into (up to) four smaller rectangles. The node's
 * rectangle is split evenly (or as close to evenly as possible)
 * along both horizontal and vertical axes. If an even split along
 * the vertical axis is not possible, the extra line will be included
 * in the left side; If an even split along the horizontal axis is not
 * possible, the extra line will be included in the upper side.
 * If a single-pixel-wide rectangle needs to be split, the NE and SE children
 * will be null; likewise if a single-pixel-tall rectangle needs to be split,
 * the SW and SE children will be null.
 *
 * In this way, each of the children's rectangles together will have coordinates
 * that when combined, completely cover the original rectangle's image
 * region and do not overlap.
 */
QTree::QTree(const PNG& imIn) : root(nullptr) {
	width = imIn.width();
	height = imIn.height();
	root = BuildNode(imIn, make_pair(0, 0), make_pair(width - 1, height - 1));
}
	


/**
 * Overloaded assignment operator for QTrees.
 * Part of the Big Three that we must define because the class
 * allocates dynamic memory. This depends on your implementation
 * of the copy and clear funtions.
 *
 * @param rhs The right hand side of the assignment statement.
 */
QTree& QTree::operator=(const QTree& rhs) {
	if (this != &rhs) {
        Clear();
        Copy(rhs);
    }

    return *this;
	
}

/**
 * Render returns a PNG image consisting of the pixels
 * stored in the tree. may be used on pruned trees. Draws
 * every leaf node's rectangle onto a PNG canvas using the
 * average color stored in the node.
 *
 * For up-scaled images, no color interpolation will be done;
 * each rectangle is fully rendered into a larger rectangular region.
 *
 * @param scale multiplier for each horizontal/vertical dimension
 * @pre scale > 0
 */
PNG QTree::Render(unsigned int scale) const {

    PNG img(width * scale, height * scale);
    RenderHelper(root, img, scale);
    return img;
}

/**
 *  Prune function trims subtrees as high as possible in the tree.
 *  A subtree is pruned (cleared) if all of the subtree's leaves are within
 *  tolerance of the average color stored in the root of the subtree.
 *  NOTE - you may use the distanceTo function found in RGBAPixel.h
 *  Pruning criteria should be evaluated on the original tree, not
 *  on any pruned subtree. (we only expect that trees would be pruned once.)
 *
 * You may want a recursive helper function for this one.
 *
 * @param tolerance maximum RGBA distance to qualify for pruning
 * @pre this tree has not previously been pruned, nor is copied from a previously pruned tree.
 */
void QTree::Prune(double tolerance) {

	PruneHelper(root, tolerance);
	
}

/**
 *  FlipHorizontal rearranges the contents of the tree, so that
 *  its rendered image will appear mirrored across a vertical axis.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  After flipping, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel wide rectangles have
 *  null eastern children
 *  (i.e. after flipping, a node's NW and SW pointers may be null, but
 *  have non-null NE and SE)
 * 
 *  You may want a recursive helper function for this one.
 */
void QTree::FlipHorizontal() {
    
    if (root != nullptr) {
        FlipHorizontalHelper(root);
    }
    
	
}

/**
 *  RotateCCW rearranges the contents of the tree, so that its
 *  rendered image will appear rotated by 90 degrees counter-clockwise.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  Note that this may alter the dimensions of the rendered image, relative
 *  to its original dimensions.
 *
 *  After rotation, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel tall or wide rectangles
 *  have null eastern or southern children
 *  (i.e. after rotation, a node's NW and NE pointers may be null, but have
 *  non-null SW and SE, or it may have null NW/SW but non-null NE/SE)
 *
 *  You may want a recursive helper function for this one.
 */
void QTree::RotateCCW() {
	unsigned int oldWidth = width;
    unsigned int oldHeight = height;

    // Swap width and height
    width = oldHeight;
    height = oldWidth;

    if (root != nullptr) {
        RotateCCWHelper(root, oldWidth, oldHeight);
    }
}

/**
 * Destroys all dynamically allocated memory associated with the
 * current QTree object. Complete for PA3.
 * You may want a recursive helper function for this one.
 */
void QTree:: Clear() {

	ClearHelper(root);
    root = nullptr;
	
}

/**
 * Copies the parameter other QTree into the current QTree.
 * Does not free any memory. Called by copy constructor and operator=.
 * You may want a recursive helper function for this one.
 * @param other The QTree to be copied.
 */
void QTree::Copy(const QTree& other) {
    Clear();
	root = CopyHelper(other.root);
	
}

/**
 * Private helper function for the constructor. Recursively builds
 * the tree according to the specification of the constructor.
 * @param img reference to the original input image.
 * @param ul upper left point of current node's rectangle.
 * @param lr lower right point of current node's rectangle.
 */
Node* QTree::BuildNode(const PNG& img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr) {
	if (ul.first == lr.first && ul.second == lr.second) {
		RGBAPixel* pixel = img.getPixel(ul.first, ul.second);
		return new Node(ul, lr, *pixel);
	} else {
		Node* node = new Node(ul, lr, RGBAPixel());

		int midWidth = (ul.first + lr.first) / 2;
		int midHeight = (ul.second + lr.second) / 2;

		node->NW = BuildNode(img, ul, make_pair(midWidth, midHeight));

		if (lr.first > midWidth) {
			node->NE = BuildNode(img, make_pair(midWidth + 1, ul.second), make_pair(lr.first, midHeight));
		}
		if (lr.second > midHeight) {
			node->SW = BuildNode(img, make_pair(ul.first, midHeight + 1), make_pair(midWidth, lr.second));
		} 
		if (lr.first > midWidth && lr.second > midHeight) {
			node->SE = BuildNode(img, make_pair(midWidth + 1, midHeight + 1), lr);
		}

		node->avg = AverageColor(node);
		return node;
	}
}

/*********************************************************/
/*** IMPLEMENT YOUR OWN PRIVATE MEMBER FUNCTIONS BELOW ***/
/*********************************************************/

void QTree::RenderHelper(Node* node, PNG& img, unsigned int scale) const {
    if (node == nullptr) {
        return;
    }

    if (node->NW == nullptr && node->NE == nullptr && node->SW == nullptr && node->SE == nullptr) {
        for (unsigned int x = (node->upLeft.first * scale); x <= ((node->lowRight.first + 1) * scale); x++) {
            for (unsigned int y = (node->upLeft.second * scale); y <= ((node->lowRight.second + 1) * scale); y++) {
                RGBAPixel *pixel = img.getPixel(x, y);
                *pixel = node->avg;
            }
        }
    } else {
        RenderHelper(node->NW, img, scale);
        RenderHelper(node->NE, img, scale);
        RenderHelper(node->SW, img, scale);
        RenderHelper(node->SE, img, scale);
    }
}


void QTree::FlipHorizontalHelper(Node* node) {
	if (node == nullptr)
		return;
	
    Node* temp = node->NW;
    node->NW = node->NE;
    node->NE = temp;

    temp = node->SW;
    node->SW = node->SE;
    node->SE = temp;
	
	unsigned int newUlX = width - 1 -  node->lowRight.first;
	unsigned int newLrX = width - 1 - node->upLeft.first;

	node->upLeft.first = newUlX;
	node->lowRight.first = newLrX;

    
    FlipHorizontalHelper(node->NW);
	FlipHorizontalHelper(node->NE);
	FlipHorizontalHelper(node->SW);
	FlipHorizontalHelper(node->SE);
	
	
}



void QTree::RotateCCWHelper(Node* node, unsigned int oldWidth, unsigned int oldHeight) {
    if (node == nullptr)
        return;

    // Rotating the children pointers counterclockwise
    Node* temp = node->NW;
    node->NW = node->NE;
    node->NE = node->SE;
    node->SE = node->SW;
    node->SW = temp;

    // Update the positions of upLeft and lowRight for each quadrant
    unsigned int newUlX = node->upLeft.second;
    unsigned int newUlY = oldWidth - 1 - node->lowRight.first;
    unsigned int newLrX = node->lowRight.second;
    unsigned int newLrY = oldWidth - 1 - node->upLeft.first;

    node->upLeft = make_pair(newUlX, newUlY);
    node->lowRight = make_pair(newLrX, newLrY);

    // Recursively rotate the child nodes
    RotateCCWHelper(node->NW, oldWidth, oldHeight);
    RotateCCWHelper(node->NE, oldWidth, oldHeight);
    RotateCCWHelper(node->SW, oldWidth, oldHeight);
    RotateCCWHelper(node->SE, oldWidth, oldHeight);
}


void QTree::ClearHelper(Node* node) {
    if (node == nullptr) {
        return;
    }

    ClearHelper(node->NW);
    ClearHelper(node->NE);
    ClearHelper(node->SW);
    ClearHelper(node->SE);

    delete node;
}

Node* QTree::CopyHelper(const Node* otherNd) {
    if (otherNd == nullptr) {
        return nullptr;
    }


    Node* newNode = new Node(otherNd->upLeft, otherNd->lowRight, otherNd->avg);
    
    newNode->NW = CopyHelper(otherNd->NW);
    newNode->NE = CopyHelper(otherNd->NE);
    newNode->SW = CopyHelper(otherNd->SW);
    newNode->SE = CopyHelper(otherNd->SE);

    return newNode;
}

RGBAPixel QTree::AverageColor(Node* node) {
    RGBAPixel nwPixel = RGBAPixel(0,0,0);
	RGBAPixel nePixel = RGBAPixel(0,0,0);
	RGBAPixel sePixel = RGBAPixel(0,0,0);
	RGBAPixel swPixel = RGBAPixel(0,0,0);
	int nwArea = 0;
	int neArea = 0;
	int seArea = 0;
	int swArea = 0;

	if(node->NW) {
		nwPixel = node->NW->avg;
		nwArea = (node->NW->lowRight.first - node->NW->upLeft.first + 1) * (node->NW->lowRight.second - node->NW->upLeft.second + 1);
	}
	if(node->NE) {
		nePixel = node->NE->avg;
		neArea = (node->NE->lowRight.first - node->NE->upLeft.first + 1) * (node->NE->lowRight.second - node->NE->upLeft.second + 1);	
	}
	if (node->SE) {
		sePixel = node->SE->avg;
		seArea = (node->SE->lowRight.first - node->SE->upLeft.first + 1) * (node->SE->lowRight.second - node->SE->upLeft.second + 1);
	} 
	if (node->SW) {
 		swPixel = node->SW->avg;
		swArea = (node->SW->lowRight.first - node->SW->upLeft.first + 1) * (node->SW->lowRight.second - node->SW->upLeft.second + 1);
	}

	int totalArea = (node->lowRight.first - node->upLeft.first + 1) * (node->lowRight.second - node->upLeft.second + 1);

	int averageRed = ((nwArea * nwPixel.r) + (neArea * nePixel.r) + (swArea * swPixel.r) + (seArea * sePixel.r)) / totalArea;
	int averageGreen = ((nwArea * nwPixel.g) + (neArea * nePixel.g) + (swArea * swPixel.g) + (seArea * sePixel.g)) / totalArea;
	int averageBlue = ((nwArea * nwPixel.b) + (neArea * nePixel.b) + (swArea * swPixel.b) + (seArea * sePixel.b)) / totalArea;

	return RGBAPixel(averageRed, averageGreen, averageBlue);
}

bool QTree::CanPrune(Node* root, Node* node, double tolerance) const {
    if (node == nullptr) {
        return true;
    }

    if (node->NW == nullptr && node->NE == nullptr && node->SW == nullptr && node->SE == nullptr) { 
        return root->avg.distanceTo(node->avg) <= tolerance;
    }

    // Recursively check all children
    return CanPrune(root, node->NW, tolerance) && CanPrune(root, node->NE, tolerance) && CanPrune(root, node->SW, tolerance) && CanPrune(root, node->SE, tolerance);
}

void QTree::PruneSubtree(Node* node) {
    if (node == nullptr) {
        return;
    }

    ClearHelper(node->NW);
    ClearHelper(node->NE);
    ClearHelper(node->SW);
    ClearHelper(node->SE);

    node->NW = nullptr;
	node->NE = nullptr;
	node->SW = nullptr;
	node->SE = nullptr;
}

void QTree::PruneHelper(Node* node, double tolerance) {
    if (node == nullptr || (node->NW == nullptr && node->NE == nullptr && node->SW == nullptr && node->SE == nullptr)) {
        return; 
    }

    if (CanPrune(node, node, tolerance)) {
        PruneSubtree(node); 
    } else {
        PruneHelper(node->NW, tolerance);
        PruneHelper(node->NE, tolerance);
        PruneHelper(node->SW, tolerance);
        PruneHelper(node->SE, tolerance);
    }
}