/*
 * Point.h
 *
 *  Created on: Sep 1, 2014
 *      Author: jhgan
 */

#ifndef POINT_H_
#define POINT_H_

#include "headers.h"

/*
 * The class of a single point.
 */
class Point {
protected:
	/*
	 *  The set of cluster IDs of which this point belongs to.
	 *  It is only used to avoid duplicate clusterIDs.
	 */
//	set<int> clusterIDs_set;
public:
	/*
	 *  The point coordinates
	 */
	//	vector<int> coords;
	int* coords;

	/*
	 *  point ID
	 */
	int id;

	/*
	 *  The ID of the cluster. This variable only works for core points.
	 */
	int clusterID;

	/*
	 *  The flag to mark whether this point is a core point.
	 *  If yes, then this value is TRUE. Otherwise, it is FALSE.
	 */
	bool isCore;

	/*
	 *  The flag to mark whether this point is processed.
	 *  If YES, then this value is TRUE. Otherwise, it is FALSE.
	 */
	bool isProcessed;

	/*
	 *  The flag to mark whether this point is currently in the queue.
	 *  If YES, then this value is TRUE. Otherwise, it is FALSE.
	 */
	bool isInQue;

	/*
	 * 	The constructor of Point.
	 * 	Parameter List:
	 * 		id: 		the ID of this point
	 * 		coords: 	the coordinates
	 */
	Point(int id, int* coords);

	/*
	 *  Reset this point to the initial state.
	 */
	void reset();

	/*
	 *  Assign this point to a cluster.
	 *  Parameter List:
	 *  	clusterID: 	the ID of the cluster which you assign this point to
	 */
	void assignToCluster(int clusterID);

	/*
	 *  Clear the list of clusterIDs in this point.
	 */
	void clearClusters();

	/*
	 *  Compute the square of distance from this point to point pt.
	 *  Parameter List:
	 *  	pt:		the pointer of the target point
	 */
	double getSqrDist(Point* pt);

	/*
	 *  Compute the distance from this point to point pt.
	 *  Parameter List:
	 *  	pt:		the pointer of the target point
	 */
	double getDist(Point* pt);

	/*
	 *  Get the hashed coordinates of this point.
	 *  Parameter List:
	 *  	gridCoords:			the target place to store the resulting grid coordinates
	 *  	oneOverSideLen:		the reciprocal of the side length of the grid cells
	 */
	vector<int>& getGridCoords(vector<int>& gridCoords, double oneOverSideLen);

	/*
	 *  Show the clustering result of this point.
	 */
	void showCluster();

	/*
	 *  Show the coordinates of this point.
	 */
	void showCoords();

};

#endif /* POINT_H_ */
