/*
 * OrgDBSCAN.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "OrgDBSCAN.h"

static int processedCnt = 0;

/*
 *  Brute force approach for answering range queries.
 *  Parameter List:
 *  	q:				query point
 *  	ptList:			the set of points
 *  	r:				the radius r for range query
 *  	targetPlace:	the target place to store resulting points
 */
void rangeQuery_BruteForce(Point* q, vector<Point*>& ptList, double r, vector<Point*>& targetPlace) {
	int ptNum = ptList.size();
	double sqr_r = r * r;
	for (int i = 0; i < ptNum; i++) {
		if (q->getSqrDist(ptList[i]) <= sqr_r)
			targetPlace.push_back(ptList[i]);
	}
	return;
}

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
void expandCluster(vector<Point*>& seed, vector<Point*>& ptList, double r, int k, int clusterID,
		void (*rangeQuery)(Point*, vector<Point*>&, double, vector<Point*>&)) {

	vector<Point*> targetPlace;
	int num = seed.size();
	for (int i = 0; i < num; i++) {
		/*
		 *  Assign this point to the cluster.
		 */
		assignPtToCluster(seed[i], clusterID);

		if (seed[i]->isProcessed == false) {
			rangeQuery(seed[i], ptList, r, targetPlace);
			seed[i]->isProcessed = true;

			processedCnt++;
			if (processedCnt > 0 && processedCnt % 10000 == 0) {
				printf("%d pts have been processed.\n", processedCnt);
				fflush(stdout);
			}

			if ((int) (targetPlace.size()) >= k) {
				seed[i]->isCore = true;
				// expand cluster from this core point
				expandCluster(targetPlace, ptList, r, k, clusterID, rangeQuery);
			}
			targetPlace.clear();
		}
	}
}

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
void expandCluster_Space(vector<Point*>& seed, vector<Point*>& ptList, double r, int k, int clusterID,
		void (*rangeQuery)(Point*, vector<Point*>&, double, vector<Point*>&)) {

	vector<Point*> targetPlace;
	queue<Point*> seedQue;

	int num = seed.size();
	for (int i = 0; i < num; i++) {
		seedQue.push(seed[i]);
		seed[i]->isInQue = true;
	}

	Point* curPoint = NULL;
	int targetNum = 0;
	while (!seedQue.empty()) {
		curPoint = seedQue.front();
		seedQue.pop();

		/*
		 *  Assign this point to the cluster.
		 */
		assignPtToCluster(curPoint, clusterID);

		if (curPoint->isProcessed == false) {
			rangeQuery(curPoint, ptList, r, targetPlace);
			curPoint->isProcessed = true;

			processedCnt++;
			if (processedCnt > 0 && processedCnt % 10000 == 0) {
				printf("%d pts have been processed.\n", processedCnt);
				fflush(stdout);
			}

			targetNum = targetPlace.size();
			if (targetNum >= k) {
				curPoint->isCore = true;

				// Here should update the clusterID of curPoint because it is marked as a core point.
				curPoint->clusterID = clusterID;

				for (int i = 0; i < targetNum; i++) {
					if (targetPlace[i]->isInQue == false) {
						// If this point has not ever been in the the queue, then add it to the queue.
						seedQue.push(targetPlace[i]);
						targetPlace[i]->isInQue = true;
						// Record all affected points.
						seed.push_back(targetPlace[i]);
					}
				}
			}
			targetPlace.clear();
		}
	}

	num = seed.size();
	for (int i = 0; i < num; i++) {
		seed[i]->isInQue = false;
	}

}

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
		void (*rangeQuery)(Point*, vector<Point*>&, double, vector<Point*>&), int& clusterID, bool spacePrior) {
	int ptNum = ptList.size();
	vector<Point*> targetPlace;
	int seedNum = 0;

	for (int i = 0; i < ptNum; i++) {

		if (ptList[i]->isProcessed == false) {
			rangeQuery(ptList[i], ptList, r, targetPlace);
			ptList[i]->isProcessed = true;
			seedNum = targetPlace.size();

			processedCnt++;
			if (processedCnt > 0 && processedCnt % 10000 == 0) {
				printf("%d pts have been processed.\n", processedCnt);
				fflush(stdout);
			}

			if (seedNum >= k) {
				ptList[i]->isCore = true;

				/*
				 *  Assign this core point to the cluster.
				 */
				assignPtToCluster(ptList[i], clusterID);

				if (spacePrior) {
					expandCluster_Space(targetPlace, ptList, r, k, clusterID, rangeQuery);
				} else {
					expandCluster(targetPlace, ptList, r, k, clusterID, rangeQuery);
				}
				clusterID++;
			}
			targetPlace.clear();
		}
	}
}

/*
 *  The original DBSCAN clustering algorithm.
 *  Parameter List:
 *  	ptList:			the set of all the points
 *  	r:				the radius for DBSCAN
 *  	k:				the core point threshold
 *  	clusterID:		the ID of the current cluster
 *  	spacePrior:		whether use the method that save the space but increases the running time
 */
void orgDBSCAN(vector<Point*>& ptList, double r, int k, int& clusterID, bool spacePrior) {
	orgDBSCAN(ptList, r, k, &rangeQuery_BruteForce, clusterID, spacePrior);
}

/*
 *  A global Rtree object in this scope.
 */
Rtree rtree;

/*
 *  A wrapper for the member range query function of Rtree.
 */
void rangeQuery_Rtree(Point* pt, vector<Point*>& ptList, double r, vector<Point*>& targetPlace) {
	if (rtree.isValid() == false) {
		showError("Error in OrgDBSCAN: the root of R-tree is NULL!\n");
	}

	double start = 0;
	double end = 0;
	start = getCurrentTime();
	rtree.rangeQuery(pt, r, targetPlace);
	end = getCurrentTime();
	TimeRtreeRangeQuery += end - start;
}

/*
 *  The original DBSCAN with R-tree.
 */
void orgDBSCAN_Rtree(vector<Point*>& ptList, double r, int k, int& clusterID, int fanout, int bitNum, bool spacePrior) {

	double start = 0;
	double end = 0;

	start = getCurrentTime();

	rtree.bulkLoadRtree(ptList, fanout, bitNum);

	end = getCurrentTime();

	printf("R-tree construction time: %lf seconds.\n", end - start);
	fflush(stdout);

	orgDBSCAN(ptList, r, k, &(rangeQuery_Rtree), clusterID, spacePrior);

	printf("Total range queries time: %.9lf seconds.\n", TimeRtreeRangeQuery);
	printf("Total number of mbr's opened: %lld\n", TotalOpenedMbrNum);
	printf("Total number of range queries = %lld\n", TotalRangeQueryNum);
	printf("TotalDistComputation = %lld\n", TotalDistComputation);
//	printf("Total time for checking mbrs = %.9lf seconds.\n", TimeCheckMbr);
	printf("TotalStateCheck = %lld\n", TotalStateCheck);

	rtree.releaseSpace();
	if (RtreeNode::objectCnt != 0) {
		printf("Memory leak in RtreeNode! There are %d RtreeNode objects remained!\n", RtreeNode::objectCnt);
		fflush(stdout);
		showError("");
	}

}
