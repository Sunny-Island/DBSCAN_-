/*
 * Cluster.cpp
 *
 *  Created on: Oct 7, 2014
 *      Author: jhgan
 */

#include "Cluster.h"

vector<set<Point*>*> CLUSTER_LIST;

/*
 *  Assign a point to the cluster.
 */
void assignPtToCluster(Point* pt, int clusterID) {
	int clusterCnt = CLUSTER_LIST.size();

	if (clusterCnt < clusterID - 1) {
		showError(
				"Error in assignPtToCluster: There must be something wrong with your clusterID!\n");
	}

	if (clusterCnt < clusterID) {
		CLUSTER_LIST.push_back(new set<Point*>);
	}

	CLUSTER_LIST[clusterID - 1]->insert(pt);

	if (pt->isCore) {
		pt->clusterID = clusterID;
	}

}

void releaseClusterList() {
	int clusterCnt = CLUSTER_LIST.size();
	for (int i = 0; i < clusterCnt; i++) {
		CLUSTER_LIST.clear();
		delete CLUSTER_LIST[i];
		CLUSTER_LIST[i] = NULL;
	}
	CLUSTER_LIST.clear();
}

void resizeClusterList(int clusterCnt) {
	// Release and clear the current cluster list first.
	releaseClusterList();

	for (int i = 0; i < clusterCnt; i++) {
		CLUSTER_LIST.push_back(new set<Point*>);
	}
}
