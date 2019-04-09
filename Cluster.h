/*
 * Cluster.h
 *
 *  Created on: Oct 7, 2014
 *      Author: jhgan
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include "Point.h"
#include <vector>
#include <set>
using namespace std;

/*
 *  The list of resulting clusters.
 */
extern vector<set<Point*>* > CLUSTER_LIST;


/*
 *  Assign a point to the cluster.
 */
void assignPtToCluster(Point* pt, int clusterID);


/*
 *  Release and clear the cluster list.
 */
void releaseClusterList();

/*
 *  Resize the cluster list to be the specified size.
 */
void resizeClusterList(int clusterCnt);


#endif /* CLUSTER_H_ */
