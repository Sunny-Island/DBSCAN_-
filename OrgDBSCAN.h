/*
 * OrgDBSCAN.h
 *
 *  Created on: Sep 15, 2014
 *      Author: jhgan
 */

#ifndef ORGDBSCAN_H_
#define ORGDBSCAN_H_

#include "headers.h"
#include "Point.h"
#include "Rtree.h"
#include "Cluster.h"

/*
 *  Brute force approach for answering range queries.
 *  Parameter List:
 *  	q:				query point
 *  	ptList:			the set of points
 *  	r:				the radius r for range query
 *  	targetPlace:	the target place to store resulting points
 */
void rangeQuery_BruteForce(Point* q, vector<Point*>& ptList, double r,
		vector<Point*>& targetPlace);

/*
 *  Expand the cluster with clusterID from a core point pt.
 *  Parameter List:
 *  	seed:		the points that are within distance r to pt
 *  	ptList:		the set of all the points
 *  	r:			the radius r for range query
 *  	k:			the core point threshold
 *  	clusterID:	the ID of the current cluster
 *  	rangeQuery:	the pointer of the range query function
 */
void expandCluster(vector<Point*>& seed, vector<Point*>& ptList, double r,
		int k, int clusterID,
		void (*rangeQuery)(Point*, vector<Point*>&, double, vector<Point*>&));

void expandCluster_Space(vector<Point*>& seed, vector<Point*>& ptList, double r,
		int k, int clusterID,
		void (*rangeQuery)(Point*, vector<Point*>&, double, vector<Point*>&));

/*
 *  The original DBSCAN clustering algorithm.
 *  Parameter List:
 *  	ptList:			the set of all the points
 *  	r:				the radius for DBSCAN
 *  	k:				the core point threshold
 *  	rangeQuery:		the pointer of the rang query function
 *  	clusterID:		the ID of the current cluster
 *  	spacePrior:		whether use the method that save the space but increases the running time
 */
void orgDBSCAN(vector<Point*>& ptList, double r, int k,
		void (*rangeQuery)(Point*, vector<Point*>&, double, vector<Point*>&),
		int& clusterID, bool spacePrior = false);

/*
 *  The original DBSCAN clustering algorithm.
 *  Parameter List:
 *  	ptList:			the set of all the points
 *  	r:				the radius for DBSCAN
 *  	k:				the core point threshold
 *  	clusterID:		the ID of the current cluster
 *  	spacePrior:		whether use the method that save the space but increases the running time
 */
void orgDBSCAN(vector<Point*>& ptList, double r, int k, int& clusterID,
		bool spacePrior = false);

/*
 *  The original DBSCAN with R-tree.
 */
void orgDBSCAN_Rtree(vector<Point*>& ptList, double r, int k, int& clusterID,
		int fanout, int bitNum, bool spacePrior = false);

#endif /* ORGDBSCAN_H_ */
