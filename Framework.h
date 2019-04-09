/*
 * Framework.h
 *
 *  Created on: Sep 4, 2014
 *      Author: jhgan
 */

#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include "GridCell.h"
#include "ToolFunctions.h"
#include "headers.h"

class Framework {
protected:
	/*
	 *  The only level-0 grid cell.
	 */
	GridCell* root;

	/*
	 *  The number of clusters.
	 */
	int clusterCnt;


	/*
	 *  Expand a new connected component by DFS starting from a given grid cell.
	 *  Parameter List:
	 *  	g:			the pointer of the given grid cell
	 *  	clusterID:	the ID of the current cluster
	 */
	void expandConnectedComponet(GridCell* g, int clusterID);

	/*
	 *  Check whether there already exists an edge between the two given grid cells.
	 *  If YES, return TRUE. Otherwise, return FALSE.
	 */
//	bool edgeExists(GridCell* g1, GridCell* g2);
public:
	Framework();
	~Framework();

	/*
	 *  Release the space of an object of Framework.
	 */
	void releaseSpace();

	/*
	 *  Return the number of clusters.
	 */
	int getClusterCnt();

	/*
	 *  Return the point list.
	 */
	virtual vector<Point*>& getPtList()= 0;

	/*
	 ******************************************************************
	 *  The following functions are for DBSCAN or approximate DBSCAN
	 ******************************************************************
	 */

	/*
	 * Identify all the core points.
	 */
	virtual void identifyCorePts() = 0;

	/*
	 *  Construct the grid graph by adding edges between grid cells.
	 */
	virtual void constructGridGraph()=0;

	/*
	 *  Construct the grid graph by adding edges between grid cells.
	 *  Parameter List:
	 *  	edgeExists:		a function pointer for checking whether there already exists an edge between the two given grid cells
	 *  					If YES, return TRUE. Otherwise, return FALSE.
	 *  	edgeAddable:	a function pointer for checking whether it can add an edge between the two given grid cells
	 *  					If YES, return TRUE. Otherwise, return FALSE.
	 */
	void constructGridGraph(bool (*edgeExists)(GridCell*, GridCell*),
			bool (*edgeAddable)(GridCell*, GridCell*, double));

	/*
	 *  Construct the grid graph implicitly and directly cluster all the core points at the same time.
	 */
	void constructGridGraph_SpacePrior(bool (*edgeAddable)(GridCell*, GridCell*, double));

	/*
	 *  Construct the grid graph by adding edges between grid cells with a brute force algorithm for edgeExists in default.
	 *  Parameter List:
	 *  	edgeAddable:	a function pointer for checking whether it can add an edge between the two given grid cells
	 *  					If YES, return TRUE. Otherwise, return FALSE.
	 */
	void constructGridGraph(bool (*edgeAddable)(GridCell*, GridCell*, double));

	/*
	 *  Construct the grid graph progressively and directly cluster all the core points at the same time.
	 */
	void constructGridGraph_Progressive(bool (*edgeExists)(GridCell*, GridCell*),
			bool (*edgeAddable)(GridCell*, GridCell*, double));

	/*
	 *  Cluster all the core points by computing the connected components of the resulting grid graph.
	 */
	virtual void clusterCorePts()=0;

	/*
	 *  The original version of the function clusterCorePts().
	 */
	void clusterCorePts_Org();

	/*
	 * 	Assign all the non-core points to clusters.
	 */
	virtual void assignNonCorePts()=0;

	/*
	 *  The whole clustering process.
	 */
	void clustering();

};

#endif /* FRAMEWORK_H_ */
