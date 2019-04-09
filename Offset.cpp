/*
 * Offset.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "Offset.h"

/*****************************************************************************
 *  The following variables are for getting neighboring cells
 *****************************************************************************/
/*
 *  The recurrence function for computing all the valid offsets.
 *  Parameter List:
 *  	dim:		the dimensionality of points
 *  	offNum:		the number of possible grid cells in single direction
 *  	dLev:		the dLev-th dimension
 *  	cur:		the current testing offset
 *  	recSqrLen:	the square sum of all the current coordinates of cur
 *  	offsetList:	the list of all valid offsets disregarding the symmetric
 */
void recurrence(int dim, int offNum, int dLev, vector<int>& cur, int recSqrLen,
		vector<vector<int>*>& offsetList) {
	if (dLev == dim) {
		// Add the 0 offset value to the last coordinate.
		vector<int>* offset = new vector<int>(cur);
		offset->push_back(0);
		offsetList.push_back(offset);

		for (int i = 0; i < offNum; i++) {
			if (recSqrLen + i * i < dim) {
				vector<int>* offset1 = new vector<int>(cur);
				offset1->push_back(i + 1);
				offsetList.push_back(offset1);
			} else
				break;
		}
	} else {
		vector<int>* recur = new vector<int>(cur);
		recur->push_back(0);
		recurrence(dim, offNum, dLev + 1, *recur, recSqrLen, offsetList);

		int temp = recSqrLen;
		for (int i = 0; i < offNum; i++) {
			recSqrLen = temp;
			recSqrLen += i * i;
			if (recSqrLen < dim) {
				recur = new vector<int>(cur);
				recur->push_back(i + 1);
				recurrence(dim, offNum, dLev + 1, *recur, recSqrLen,
						offsetList);
				// release the space
				recur->clear();
				delete (recur);
			} else
				break;
		}
	}
}

/*
 *  The function for computing the offset list.
 *  Parameter List:
 *  	dim:		the dimensionality of the points
 *  	offsetList:	the list of all valid offsets disregarding the symmetric
 */
void computeOffsetList(int dim, vector<vector<int>*>& offsetList) {
	int offNum = ceil(sqrt(dim));
	vector<int> cur;
	recurrence(dim, offNum, 1, cur, 0, offsetList);
}

/*
 *  Check whether the two given grid cells are neighbors.
 *  Parameter List:
 *  	g1:		the coordinates of the first given grid cell
 *  	g2: 	the coordinates of the second given grid cell
 *  	dim:	the dimensionality of points
 *  Return:
 *  	If they are neighbors, return TRUE. Otherwise, return FALSE.
 */
bool isNeighbor(vector<int>& g1, vector<int>& g2, int dim) {

	// the square of minimum distance
	int sqr_minDist = 0;
	int temp = 0;
	for (int i = 0; i < dim; i++) {
		temp = 0;
		if (g1[i] + 1 < g2[i]) {
			temp = (g2[i] - g1[i] - 1);
		} else if (g2[i] + 1 < g1[i]) {
			temp = (g1[i] - g2[i] - 1);
		}
		sqr_minDist += temp * temp;
	}
	// The two grid cells are neighbor if and only if sqr_minDist < dim.
	return sqr_minDist < dim;
}

/*
 *  Get the pair of closest corners such that one is from g1 and one is from g2.
 *  Parameter List:
 *  	g1: 		the coordinates of the first given grid cell
 *  	g2: 		the coordinates of the second given grid cell
 *  	dim:		the dimensionality of points and grid cells
 *  	sideLen:	the sideLen of the grid cells
 *  	c1:			the target place to store the coordinates in the data space of the corner of g1
 *  	c2:			the target place to store the coordinates in the data space of the corner of g2
 */
void getClosestCorners(vector<int>& g1, vector<int>& g2, int dim,
		double sideLen, vector<double>& c1, vector<double>& c2) {
	c1.clear();
	c1.resize(dim);

	c2.clear();
	c2.resize(dim);

	for (int i = 0; i < dim; i++) {
		if (g1[i] + 1 <= g2[i]) {
			c1[i] = g1[i] + 1;
			c2[i] = g2[i];
		} else if (g2[i] + 1 <= g1[i]) {
			c1[i] = g1[i];
			c2[i] = g2[i] + 1;
		} else {
			c1[i] = g1[i];
			c2[i] = g2[i];
		}

		c1[i] *= sideLen; // the coordinate in the data space
		c2[i] *= sideLen; // the coordinate in the data space
	}
	return;
}

/*****************************************************************************/

