/*
 * Evaluation.h
 *
 *  Created on: Sep 24, 2014
 *      Author: jhgan
 */

#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "headers.h"
#include "FileIO.h"
#include "Cluster.h"

/*
 *********************************************************************************************
 *  This part is for evaluating the correctness and approximation of the clustering results.
 *********************************************************************************************
 */

/*
 *********************************************************************************************
 *  This part is for the case that we maintain a global cluster list for all the points together.
 *********************************************************************************************
 */


/*
 * 	Construct the mapping from core point ID to cluster ID with CLUSTER_LIST.
 * 	Parameter List:
 * 		ptCltMapping:		the target place to store the mapping from core point ID to cluster ID.
 */
void constructPtClusterMapping(map<int, int>& ptCltMapping);

/*
 *  To check whether the current clustering result is the same as that generated by the brute force DBSCAN.
 */
bool isCorrect(char* gtFilePath);

/*
 *  Write the clustering result into target files, each cluster per file.
 *  Parameter List:
 *  	targetFolder:		the folder path of the target files
 */
void writeClusterResultToFile(char* targetFolder);


/*
 *********************************************************************************************
 *  This part is for the case that each point has a list of cluster IDs.
 *********************************************************************************************
 */

/*
 *  Transform the point list after clustering into cluster list.
 *  Parameter List:
 *  	ptList:				the point list after clustering
 *  	clusterCnt:			the number of clusters
 *  	clusterList:		the target place to store the cluster list
 *  	ptCltMapping:		the mapping from core point ID to cluster ID.
 */
void transformToClusterList(vector<Point*>& ptList, int clusterCnt,
		vector<vector<Point*> >& clusterList, map<int, int>& ptCltMapping);

/*
 *  To check whether the current clustering result is the same as that generated by the brute force DBSCAN.
 *  Parameter List:
 *  	gtFilePath:			the file path of the ground truth file
 *  	ptList:				the point list after clustering
 *  	clusterCnt:			the number of clusters
 *  Return:
 *  	If the current clustering result is the same as ground truth, return TRUE. Otherwise, return FALSE.
 */
bool isCorrect(char* gtFilePath, vector<Point*>& ptList, int clusterCnt);

/*
 *  Write the clustering result into target files, each cluster per file.
 *  Parameter List:
 *  	targetFolder:		the folder path of the target files
 *  	ptList:				the point list after clustering
 *  	clusterCnt:			the number of clusters
 */
void writeClusterResultToFile(char* targetFolder, vector<Point*>& ptList,
		int clusterCnt);

#endif /* EVALUATION_H_ */
