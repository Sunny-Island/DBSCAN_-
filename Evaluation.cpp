/*
 * Evaluation.cpp
 *
 *  Created on: Sep 24, 2014
 *      Author: jhgan
 */

#include "Evaluation.h"

/*
 *********************************************************************************************
 *  This part is for the case that we maintain a global cluster list for all the points together.
 *********************************************************************************************
 */

void constructPtClusterMapping(map<int, int>& ptCltMapping) {
	vector<set<Point*>*>& clusterList = CLUSTER_LIST;
	int clusterCnt = clusterList.size();

	for (int i = 0; i < clusterCnt; i++) {
		set<Point*>* cluster = clusterList[i];
		set<Point*>::iterator it = cluster->begin();
		while (it != cluster->end()) {
			if ((*it)->isCore) {
				ptCltMapping.insert(std::pair<int, int>((*it)->id, (*it)->clusterID));
			}
			it++;
		}
	}

}

bool isCorrect(char* gtFilePath) {

	vector<set<Point*>*>& clusterList = CLUSTER_LIST;
	int clusterCnt = clusterList.size();

	set<int> gtCluster;
	bool rtn = true;

	// Read the ground truth from file.
	//	readGroundTruth(gtClusterList, representCorePtList, gtFilePath);

	printf("Start comparing with the ground truth.\n");

	FILE* file = fopen(gtFilePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function readGroundTruth:\nFile open failed! Please check your file path!\n");
	}

	int gtClusterCnt = 0;
	fscanf(file, "%d\n", &gtClusterCnt);

	if (clusterCnt != (int) gtClusterCnt) {
		printf("=============================================\n");
		printf("The cluster number is not correct!\n");
		printf("Your clustering result has %d clusters while the ground truth has %d clusters.\n",
				clusterCnt, gtClusterCnt);
		printf("=============================================\n\n");
		return false;
	}

	int representCoreID = 0;
	int clPtID = -1;
	int ptNum = 0;

	// Construct the mapping from core point ID to cluster ID.
	map<int, int> ptCltMapping;
	constructPtClusterMapping(ptCltMapping);

	// Start comparing the clustering result with ground truth.
	int myClID = -1;
	int mysize = 0;

	for (int i = 0; i < clusterCnt; i++) {
		gtCluster.clear();
		fscanf(file, "%d\t", &(representCoreID));
		fscanf(file, "%d\t", &ptNum);
		for (int j = 0; j < ptNum; j++) {
			fscanf(file, "%d ", &clPtID);
			gtCluster.insert(clPtID);
		}

		// Get the ID of my clustering result corresponding to this core point ID.
		if (ptCltMapping.find(representCoreID) == ptCltMapping.end()) {
			printf("=============================================\n");
			printf("Point %d is a core point but it is not detected by your algorithm!\n",
					representCoreID);
			printf("=============================================\n\n");
			fflush(stdout);
			rtn = false;
			//			return false;
		} else {
			myClID = ptCltMapping[representCoreID];

			set<Point*>* myCluster = clusterList[myClID - 1];
			mysize = myCluster->size();
			if (mysize != (int) gtCluster.size()) {
				printf("=============================================\n");
				printf("The sizes of the No.%d cluster are not the same!\n", i + 1);
				printf("Your cluster has %d pts while the ground truth has %d pts.\n", mysize,
						(int) gtCluster.size());
				printf("=============================================\n\n");
				fflush(stdout);
				rtn = false;
				//			return false;
			}

			set<Point*>::iterator it = myCluster->begin();

			while (it != myCluster->end()) {
				if (gtCluster.find((*it)->id) == gtCluster.end()) {
					printf("There is an unexpected point in the No.%d cluster!\n", i + 1);
					fflush(stdout);
					rtn = false;
					break;
				}
				it++;
			}
		}
	}

	printf("Finish comparing with the ground truth.\n");

	fclose(file);
	return rtn;
}

void writeClusterResultToFile(char* targetFolder) {
	mkdir(targetFolder, 0777);

	vector<set<Point*>*>& clusterList = CLUSTER_LIST;
	int clusterCnt = clusterList.size();

	char filePath[100];
	for (int i = 0; i < clusterCnt; i++) {
		sprintf(filePath, "%s/Cluster_%d", targetFolder, i + 1);
		writeClusterToFile(filePath, clusterList[i]);
	}
}
