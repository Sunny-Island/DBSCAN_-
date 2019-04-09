/*
 * GriDBSCAN.h
 *
 *  Created on: Sep 15, 2014
 *      Author: jhgan
 */

#ifndef GRIDBSCAN_H_
#define GRIDBSCAN_H_

#include "OrgDBSCAN.h"
#include "GridCell.h"
#include "Cluster.h"

/*
 *  The class for GriDBSCAN algorithm.
 */
class GriDBSCAN {
protected:
	/*
	 *  The root for maintaining all the non-empty grid cells in its hash table.
	 *  It can be regarded as a level-0 grid cell.
	 */
	GridCell* root;

	/*
	 *  The list of all the points.
	 */
	vector<Point*> ptList;

	/*
	 *  The radius for DBSCAN.
	 */
	double r;

	/*
	 *  The core point threshold for DBSCAN.
	 */
	int k;

	/*
	 *  The dimensionality of points.
	 */
	int dim;

	/*
	 *  The side length of each grid cell.
	 */
	int sideLen;

	/*
	 *  The fan out of the R-tree used in original DBSCAN.
	 */
	int fanout;

	/*
	 *  The number of bits to encode all the coordinates.
	 */
	int bitNum;


	/*
	 *  The total number of clusters among all the enclosures.
	 */
	int clusterCnt;

	/*
	 *  Compute the seed of offsets for a given point pt.
	 *  Parameter List:
	 *  	pt:		 		the given point
	 *  	gridCoords:		the coordinates of the grid cell where pt lies
	 *  	targetPlace:	the target place to store the offset seed
	 *  Return:
	 *  	targetPlace
	 */
	vector<int>& computeOffsetSeed(Point* pt, vector<int>& gridCoords,
			vector<int>& targetPlace);

	/*
	 *  Enumerate all the offsets by the offset seed and assign pt to those non-empty grid cells
	 *  at the offset from the grid cell where pt lies.
	 *  Parameter List:
	 *  	pt:				the given point
	 *  	g:				the grid cell where pt lies
	 *  	offsetSeed:		the offset seed of pt
	 *  	dLev:			the current dimension in the recursion
	 */
	void enumerateOffsets(Point* pt, GridCell* g, vector<int>& offsetSeed,
			int dLev);

	/*
	 *  Assign a given point to all the enclosures, where pt lies, of non-empty grid cells.
	 *  Parameter List:
	 *  	pt:				the given point
	 *  	g:				the grid cell where pt lies
	 */
	void assignPtToEnclosure(Point* pt, GridCell* g);

	/*
	 *  Run original DBSCAN on a given grid cell.
	 *  Parameter List:
	 *  	g:				the given grid cell
	 *  	r:				the radius for DBSCAN
	 *  	k:				the core point threshold for DBSCAN
	 *  	clusterStartID:	the start ID of clusters for this grid cell
	 */
	void runOrgDBSCAN(GridCell* g, double r, int k, int& clusterStartID);

public:
	/*
	 *  The constructor of GriDBSCAN.
	 *  Parameter List:
	 *  	ptList:			the set of all points
	 *  	r:				the radius for DBSCAN
	 *  	k:				the core point threshold for DBSCAN
	 *  	dim:			the dimensionality of points
	 *  	sideLen:		the specified side length for the grid cells
	 *  	fanout:			the fan out of the R-tree used in original DBSCAN
	 *  	bitNum:			the number of bit to encode all the coordintes
	 */
	GriDBSCAN(vector<Point*>& ptList, double r, int k, int dim, double sideLen, int fanout = 10, int bitNum= 32);

	virtual ~GriDBSCAN();

	/*
	 *  Release the space of this object.
	 */
	void releaseSpace();

	/*
	 *  Return the point list.
	 */
	vector<Point*>& getPtList();

	/*
	 *  Return the number of clusters.
	 */
	int getClusterCnt();


	/*
	 *  Hash all the points into grid cells.
	 */
	void constructGridCells();

	/*
	 *  Construct the r-enclosure for each grid cell.
	 */
	void constructEnclosures();

	/*
	 *  Cluster the points in r-enclosure of each grid cell by original DBSCAN.d
	 */
	void clusterEnclosures();

	/*
	 *  Merge all the clusters of each enclosure together.
	 */
	void mergeClusters();

	/*
	 *  Merge all the clusters of each enclosure together by the method of the original paper.
	 */
//	void mergeClusters_org();

	/*
	 *  Merge all the clusters of each enclosure together by the method of the original paper.
	 */
	void mergeClusters_org_ByGlobalList();

	/*
	 *  Output and display clustering results.
	 */
//	void displayClusters();

	/*
	 *  Run the GriDBSCAN clustering algorithm.
	 */
	void clustering();

};

#endif /* GRIDBSCAN_H_ */
