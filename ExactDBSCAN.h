/*
 * DBSCAN.h
 *
 *  Created on: Sep 2, 2014
 *      Author: jhgan
 */

#ifndef DBSCAN_H_
#define DBSCAN_H_

#include "Framework.h"

class ExactDBSCAN: public Framework {
protected:

	/*
	 *  The list of the pointers of points.
	 */
	vector<Point*> ptList;


	/*
	 *  Check whether the given non-core point can be assigned to the cluster of the given grid cell.
	 *  If YES, return TRUE. Otherwise, return FALSE.
	 */
	bool assignable(Point* p, GridCell* g, double r);

	/*
	 *  A function pointer for adding edges to construct grid graph.
	 */
	bool (*edgeAddable)(GridCell*, GridCell*, double);

public:
	ExactDBSCAN(vector<Point*>& ptList, bool (*edgeAddable)(GridCell*, GridCell*, double));
	~ExactDBSCAN();


	/*
	 ******************************************************************
	 *  The following functions are for DBSCAN
	 ******************************************************************
	 */

	/*
	 *  Return the point list.
	 */
	vector<Point*>& getPtList();

	/*
	 * Identify all the core points.
	 */
	void identifyCorePts();

	/*
	 *  Construct the grid graph by adding edges between grid cells.
	 */
	void constructGridGraph();

	/*
	 *  Cluster all the core points by computing the connected components of the resulting grid graph.
	 */
	void clusterCorePts();

	/*
	 * 	Assign all the non-core points to clusters.
	 */
	void assignNonCorePts();


};

#endif /* DBSCAN_H_ */
