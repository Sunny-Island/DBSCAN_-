/*
 * RtreeNode.h
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#ifndef RTREENODE_H_
#define RTREENODE_H_

#include "Rectangle.h"
#include "Point.h"

/*
 *  The class for nodes of R-tree.
 */
class RtreeNode {
protected:

	/*
	 *  The level of this node in the R-tree. All the leaves are at level-0.
	 */
	int level;

	/*
	 *  The minimum bounding rectangle of the child nodes.
	 */
	Rectangle mbr;

	/*
	 *  The list of child nodes.
	 */
	vector<RtreeNode*> childNodes;

	/*
	 *  The data object in of this node if this is a leaf.
	 */
	Point* pt;

public:

	/*
	 *  The number of the objects generated.
	 */
	static int objectCnt;

	/*
	 *  Construct an empty node.
	 */
	RtreeNode();

	/*
	 *  Construct a leaf node by a given point pt.
	 */
	RtreeNode(Point* pt);

	~RtreeNode();


	/*
	 *  Check whether this node is a leaf.
	 *  If YES, return TRUE. Otherwise, return FALSE.
	 */
	bool isLeaf();

	/*
	 *  Get the point from the this node. If this node is not a leaf, return NULL.
	 */
	Point*	getPoint();

	/*
	 *  Get the list of child nodes of this node.
	 */
	vector<RtreeNode*>& getChildNodes();

	/*
	 *  Add a child node to this node.
	 *  Parameter List:
	 *  	child:		the pointer of the given child node.
	 */
	void addChildNode(RtreeNode* child);

	/*
	 *  Check the state of the MBR of this node with the sphere centered at q with radius r.
	 *  Parameter List:
	 *  	q:			the query point
	 *  	r:			the radius of the sphere
	 *  Return:
	 *  	1:		the MBR is fully inside the query sphere
	 *  	0:		the MBR is intersected with the boundary of the query sphere
	 *  	-1:		the MBR is fully outside the query sphere
	 */
	int stateWithSphere(Point* q, double r);

	/*
	 *  Show the information of this tree node.
	 */
	void showTreeNode();

	/*
	 *  Release the space of this tree node.
	 */
	void releaseSpace();
};

#endif /* RTREENODE_H_ */
