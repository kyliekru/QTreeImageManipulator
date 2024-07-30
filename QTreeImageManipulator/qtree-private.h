/**
 * @file qtree-private.h
 * @description student declaration of private QTree functions
 *              CPSC 221 PA3
 *
 *              SUBMIT THIS FILE.
 * 
 *				Simply declare your function prototypes here.
 *              No other scaffolding is necessary.
 */

// begin your declarations below

    
void RenderHelper(Node* nd, PNG& img, unsigned int scale) const;

void FlipHorizontalHelper(Node* node);

void RotateCCWHelper(Node* node, unsigned int oldWidth, unsigned int oldHeight);

void UpdateCoordinatesForRotate(Node* node, unsigned int imageWidth, unsigned int imageHeight);

void ClearHelper(Node* nd);

Node* CopyHelper(const Node* otherNd);

RGBAPixel AverageColor(Node * node);

bool CanPrune(Node* root, Node* nd, double tolerance) const;

void PruneSubtree(Node* nd);

void PruneHelper(Node* nd, double tolerance); 
