/*
 * Rtree.h
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#ifndef RTREE_H_
#define RTREE_H_

#include "RtreeNode.h"
#include "Zorder.h"
#include <algorithm>
#include <queue>
#include <set>
using namespace std;

/*
 *  Define the element for sorting.
 */
struct RtreeElement {
	/*
	 *  The pointer of point.
	 */
	Point* pt;

	/*
	 *  The Z-order value of this point.
	 */
	Zorder zdr;

	/*
	 *  The constructor with a given point.
	 *  Parameter List:
	 *  	pt:	 		the given point
	 *  	bitNum:		the number of bits to encode each coordinate value
	 */
	RtreeElement(Point* pt, int bitNum = WORD) {
		this->pt = pt;
		encode(pt->coords, this->zdr, bitNum);
	}

	/*
	 *  Release the space.
	 */
	void releaseSpace() {
		zdr.clear();
	}

	/*
	 *  The compare function for elements.
	 */
	bool operator <(const RtreeElement* e1) {
		return compCode(this->zdr, e1->zdr);
	}

	bool operator ()(const RtreeElement* e1, const RtreeElement* e2) {
		return compCode(e1->zdr, e2->zdr);
	}
};

/*
 *  The class for R-tree.
 */
class Rtree {
protected:
	/*
	 *  The root of the R-tree.
	 */
	RtreeNode* root;

public:
	Rtree();
	~Rtree();

	/*
	 *  Bulk load construction for R-tree.
	 *  Parameter List:
	 *		ptList:			the set of all data points
	 *		fanout:			the number of child nodes of each internal node
	 *  	bitNum:		the number of bits to encode each coordinate value
	 */
	void bulkLoadRtree(vector<Point*>& ptList, int fanout, int bitNum = WORD);

	/*
	 *  Range query for a given query point with given radius.
	 *  Parameter List:
	 *  	pt:				the given query point
	 *  	r:				the given radius
	 *  	targetPlace:	the target place to store the points within the query range.
	 */
	void rangeQuery(Point* pt, double r, vector<Point*>& targetPlace);

	/*
	 *  Show the whole R-tree.
	 */
	void showRtree();

	/*
	 *  Check whether this R-tree is valid. If YES, return TRUE. Otherwise, return FALSE.
	 */
	bool isValid();


	/*
	 *  Release the space of R-tree.
	 */
	void releaseSpace();

};

#endif /* RTREE_H_ */
