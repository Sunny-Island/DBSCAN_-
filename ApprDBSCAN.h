/*
 * ApprDBSCAN.h
 *
 *  Created on: Sep 2, 2014
 *      Author: jhgan
 */

#ifndef APPRDBSCAN_H_
#define APPRDBSCAN_H_

#include "Framework.h"
#include "ToolFunctions.h"

class ApprDBSCAN: public Framework {
protected:

	/*
	 *  The list of the pointers of points.
	 */
	vector<Point*> ptList;


	/*
	 *  Identify core points by approximate counting.
	 */
	void identifyCorePts_ApprCounting();

public:
	ApprDBSCAN(vector<Point*>& ptList);
	~ApprDBSCAN();

	/*
	 ******************************************************************
	 *  The following functions are for Approximate DBSCAN
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

	/*
	 *  Compute the approximate counting result of this query point in this grid,
	 *  where the grid level should be >= 1.
	 *  Parameter List:
	 *  	q:				the center point of the query sphere
	 *  	g:				the pointer of the given grid cell
	 *  	lowestLevel: 	the level of the grid cells with the smallest side length
	 */
	void apprCounting(Point* q, GridCell* g, int lowestLevel);

	int apprCounting_BestFirst(Point* q, vector<GridCell*>& neighborList);

};

#endif /* APPRDBSCAN_H_ */
