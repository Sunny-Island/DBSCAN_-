/*
 * Offset.h
 *
 *  Created on: Sep 15, 2014
 *      Author: jhgan
 */

#ifndef OFFSET_H_
#define OFFSET_H_

#include "headers.h"

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
		vector<vector<int>*>& offsetList);

/*
 *  The function for computing the offset list.
 *  Parameter List:
 *  	dim:		the dimensionality of the points
 *  	offsetList:	the list of all valid offsets disregarding the symmetric
 */
void computeOffsetList(int dim, vector<vector<int>*>& offsetList);

/*
 *  Check whether the two given grid cells are neighbors.
 *  Parameter List:
 *  	g1:		the coordinates of the first given grid cell
 *  	g2: 	the coordinates of the second given grid cell
 *  	dim:	the dimensionality of points
 *  Return:
 *  	If they are neighbors, return TRUE. Otherwise, return FALSE.
 */
bool isNeighbor(vector<int>& g1, vector<int>& g2, int dim);


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
void getClosestCorners(vector<int>& g1, vector<int>& g2, int dim, double sideLen, vector<double>& c1, vector<double>& c2);

/*****************************************************************************/

#endif /* OFFSET_H_ */
