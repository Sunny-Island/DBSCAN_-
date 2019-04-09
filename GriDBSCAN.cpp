/*
 * GriDBSCAN.cpp
 *
 *  Created on: Sep 15, 2014
 *      Author: jhgan
 */

#include "GriDBSCAN.h"

GriDBSCAN::GriDBSCAN(vector<Point*>& ptList, double r, int k, int dim, double sideLen, int fanout,
		int bitNum) {
	this->root = new GridCell();
	GridCell::setRoot(this->root);

	// Note that the side length of the root must be double of the grid cells.
	// Because root is a level-0 grid cell whose side length is double of that of level-1 grid cells.
	this->root->setSideLength(2 * sideLen);

	this->ptList.swap(ptList);
	this->r = r;
	this->k = k;
	this->dim = dim;
	this->sideLen = sideLen;
	this->clusterCnt = 0;
	this->fanout = fanout;
	this->bitNum = bitNum;
}

GriDBSCAN::~GriDBSCAN() {
	// TODO Auto-generated destructor stub
	if (root != NULL) {
		root->releaseSpace();
		delete root;
		root = NULL;
	}

}

void GriDBSCAN::releaseSpace() {
	if (root != NULL) {
		root->releaseSpace();
		delete (root);
		root = NULL;
	}
}

vector<Point*>& GriDBSCAN::getPtList() {
	return this->ptList;
}

int GriDBSCAN::getClusterCnt() {
	return this->clusterCnt;
}

void GriDBSCAN::constructGridCells() {

	if (this->root->gridTable == NULL) {
		this->root->gridTable = new HashTable;
	}

	// Hash all the points into grid cells.
	int ptNum = this->ptList.size();

//	double start = 0;
//	double end = 0;
//	double duration = 0;

	for (int i = 0; i < ptNum; i++) {
//		start = getCurrentTime();

		// Add this point to hash table but no need to compute the neighboring cells for the level-1 grid cells.
		this->root->insertPointToTable(this->ptList[i], false);

//		end = getCurrentTime();
//		duration = end - start;
//		printf("Point ID = %d: %lf second.\n", i + 1, duration);
//		fflush(stdout);
	}
}

void GriDBSCAN::clustering() {
	double start = 0;
	double end = 0;
	double duration = 0;

	start = getCurrentTime();

	// Construct grid cells.
	this->constructGridCells();

	end = getCurrentTime();
	duration = end - start;
	printf("constructGridCells: %lf second.\n", duration);
	fflush(stdout);

	start = getCurrentTime();

	// Construct r-enclosure for each non-empty grid cell.
	this->constructEnclosures();

	end = getCurrentTime();
	duration = end - start;
	printf("constructEnclosures: %lf second.\n", duration);
	fflush(stdout);

	start = getCurrentTime();

	// Cluster each r-enclosure by original DBSCAN algorithm.
	this->clusterEnclosures();

	end = getCurrentTime();
	duration = end - start;
	printf("clusterEnclosures: %lf second.\n", duration);
	fflush(stdout);

	start = getCurrentTime();
	// Merge all those resulting clusters.
	this->mergeClusters_org_ByGlobalList();

	end = getCurrentTime();
	duration = end - start;
	printf("mergeClusters_org: %lf second.\n", duration);
	fflush(stdout);
}

/******************************************************************************
 *  The following functions are for computing enclosure for each grid cell.
 *****************************************************************************/

vector<int>& GriDBSCAN::computeOffsetSeed(Point* pt, vector<int>& gridCoords,
		vector<int>& targetPlace) {
	int dim = this->dim;
	double r = this->r;
	double sideLen = this->sideLen;
	double temp = 0;
	int direction = 0;

	for (int i = 0; i < dim; i++) {
		direction = 0;
		temp = pt->coords[i] - gridCoords[i] * sideLen;
		if (temp <= r) {
			// pt is close to the left in this dimension
			direction = -1;
		} else if (sideLen - temp <= r) {
			// pt is close to the right in this dimension
			direction = 1;
		}
		targetPlace.push_back(direction);
	}

	return targetPlace;
}

void GriDBSCAN::enumerateOffsets(Point* pt, GridCell* g, vector<int>& offsetSeed, int dLev) {
	if (dLev == this->dim - 1) {
		int dim = this->dim;
		int zeroCnt = 0;
		// If the coordinates of offsetSeed are all 0, we can skip.
		for (int i = 0; i < this->dim; i++) {
			if (offsetSeed[i] == 0) {
				zeroCnt++;
			}
		}
		if (zeroCnt == dim)
			return;

		unsigned int key = 0;

		GridCell* gTemp = NULL;
		vector<int>& gridCoords = g->getCoords();

		vector<int> target;
		target.reserve(dim + 1);

		for (int i = 0; i < dim; i++) {
			target.push_back(gridCoords[i] + offsetSeed[i]);
		}
		key = this->root->computeHashKey(target);
		target.push_back(key);
		gTemp = this->root->getCell(target);
		if (gTemp != NULL && gTemp != g) {
			// If g is non-empty, add pt to the point list of g.
			gTemp->getPtList().push_back(pt);
		}

		if (offsetSeed[dLev] != 0 && zeroCnt < dLev) {
			// This is equivalent to set offsetSeed[dLev] = 0;
			target[dLev] -= offsetSeed[dLev];
			key = this->root->computeHashKey(target);
			target[dim] = key;
			gTemp = this->root->getCell(target);
			if (gTemp != NULL && gTemp != g) {
				// If g is non-empty, add pt to the point list of g.
				gTemp->getPtList().push_back(pt);
			}
		}
		target.clear();
		return;
	}

	// recurse in the next dimension
	enumerateOffsets(pt, g, offsetSeed, dLev + 1);
	if (offsetSeed[dLev] != 0) {
		// record the current value of offsetSeed[dLev]
		int temp = offsetSeed[dLev];

		offsetSeed[dLev] = 0;
		enumerateOffsets(pt, g, offsetSeed, dLev + 1);

		// change the offsetSeed[dLev] back
		offsetSeed[dLev] = temp;
	}

}

void GriDBSCAN::assignPtToEnclosure(Point* pt, GridCell* g) {
	vector<int> offsetSeed;
	offsetSeed.reserve(this->dim);
	computeOffsetSeed(pt, g->getCoords(), offsetSeed);

	bool isDone = true;

	// If the coordinates of offsetSeed are all 0,
	// then pt does not lie in any enclosure of other grid cell.
	for (int i = 0; i < this->dim; i++) {
		if (offsetSeed[i] != 0) {
			isDone = false;
			break;
		}
	}

	if (!isDone) {
		enumerateOffsets(pt, g, offsetSeed, 0);
	}
	offsetSeed.clear();
	return;
}

void GriDBSCAN::constructEnclosures() {
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it = table->begin();
	int ptNum = 0;
	while (it != table->end()) {
		// Important! We must use getSize() instead of getPtList().size().
		// Because the ptList of the grid cell contains some points from r-enclosure.
		ptNum = it->second->getSize();
		vector<Point*>& ptList = it->second->getPtList();
		for (int i = 0; i < ptNum; i++) {
			assignPtToEnclosure(ptList[i], it->second);
		}
		it++;
	}
}

/******************************************************************************
 *  The following functions are for clustering each enclosure.
 *****************************************************************************/

void GriDBSCAN::runOrgDBSCAN(GridCell* g, double r, int k, int& clusterStartID) {
	vector<Point*>& ptList = g->getPtList();
	int ptNum = ptList.size();
	// Reset the flag of is processed.
	for (int i = 0; i < ptNum; i++) {
		ptList[i]->isProcessed = false;
	}
	orgDBSCAN_Rtree(ptList, r, k, clusterStartID, this->fanout, this->bitNum, true);
}

void GriDBSCAN::clusterEnclosures() {
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it = table->begin();
//	this->clusterCnt = 0;

	/*************** Output Codes **********************************/
	printf("There are %d enclosures.\n", (int) table->size());
	int ptNum = 0;
	int cnt = 0;
	int smallCnt = 0;
	double ratio = 0;
	double temp = 0;
	while (it != table->end()) {
		ptNum = it->second->getPtList().size();
		temp = ptNum * 1.0 / N;
		ratio += temp * temp;

		if (ptNum > 10000) {
			printf("There are %d pts in this enclosure.\n", ptNum);
			fflush(stdout);
		} else {
			smallCnt += ptNum;
		}
		cnt += ptNum;
		it++;
	}
	printf("\nThere are %d pts in total.\n", cnt);
	printf("There are %d pts in small enclosures.\n\n", smallCnt);
	printf("The ratio is %lf.\n", ratio);
	fflush(stdout);

	/*************** Output Codes **********************************/

	int clusterID = 1;

	it = table->begin();
	while (it != table->end()) {
		// Note that the cluster IDs are start from 0.
		runOrgDBSCAN(it->second, r, k, clusterID);
		it++;
	}

	this->clusterCnt = clusterID - 1;
}

/*************************************************************************************
 *  The following functions are for merging clusters by the method of the paper.
 *************************************************************************************/
/*
 *  Transform the current global cluster list into point-cluster list.
 *  Parameter List:
 *  	ptClusterList:		the target place to store the point-cluster list
 */
void transformToPointClusterList(vector<vector<int>>& ptClusterList) {
	ptClusterList.clear();
	ptClusterList.resize(N);

	vector<set<Point*>*>& clusterList = CLUSTER_LIST;
	int clusterCnt = clusterList.size();

	set<Point*>::iterator it;
	int id = 0;
	for (int i = 0; i < clusterCnt; i++) {
		it = clusterList[i]->begin();
		while (it != clusterList[i]->end()) {
			id = (*it)->id;
			ptClusterList[id - 1].push_back(i + 1);
			it++;
		}
	}
}

/*
 *  Merge the two given clusters.
 *  Parameter List:
 *  	eqList:			the equivalent list
 *  	id1:			the first given cluster ID
 *  	id2:			the second given cluster ID
 */
void mergeCluster(vector<int>& eqList, int id1, int id2) {
	int gID1 = eqList[id1 - 1];
	int gID2 = eqList[id2 - 1];
	int dirtyID, minID;
	if (gID1 < gID2) {
		dirtyID = gID2;
		minID = gID1;
	} else {
		dirtyID = gID1;
		minID = gID2;
	}

	//Update the equivalent list.
	for (int i = 0; i < (int) eqList.size(); i++) {
		if (eqList[i] == dirtyID) {
			eqList[i] = minID;
		}
	}
}

void GriDBSCAN::mergeClusters_org_ByGlobalList() {

	// Transform the current global cluster list into point-cluster list.
	vector<vector<int> > ptClusterList;
	transformToPointClusterList(ptClusterList);

	// Initialize the equivalent list.
	vector<int> eqList;
	eqList.resize(this->clusterCnt);
	for (int i = 0; i < this->clusterCnt; i++) {
		eqList[i] = i + 1;
	}
	int ptNum = this->ptList.size();
	int clNum = 0;
	int minID = this->clusterCnt + 1;
	int pos = 0;
	int ptID = 0;
	for (int i = 0; i < ptNum; i++) {
		if (ptList[i]->isCore) {
			ptID = ptList[i]->id;
			minID = this->clusterCnt + 1;
			clNum = ptClusterList[ptID - 1].size();

			// Get the minID from the eqList.
			for (int j = 0; j < clNum; j++) {
				if (eqList[ptClusterList[ptID - 1][j] - 1] < minID) {
					minID = eqList[ptClusterList[ptID - 1][j] - 1];
					pos = j; // Record the position of the clusterID with smallest groupID.
				}
			}

			for (int j = 0; j < clNum; j++) {
				if (j != pos) {
					mergeCluster(eqList, ptClusterList[ptID - 1][pos], ptClusterList[ptID - 1][j]);
				}
			}
		}
	}

	map<int, int> clusterIDMapping;
	int ID = 1;
	for (int i = 0; i < (int) eqList.size(); i++) {
		if (clusterIDMapping.find(eqList[i]) == clusterIDMapping.end()) {
			clusterIDMapping.insert(std::pair<int, int>(eqList[i], ID));
			ID++;
		}
	}

	// Update the number of total clusters.
	this->clusterCnt = clusterIDMapping.size();

	// Reset the global cluster list.
	resizeClusterList(this->clusterCnt);

	// Update the cluster info for each point.
	for (int i = 0; i < ptNum; i++) {
		ptID = ptList[i]->id;
		clNum = ptClusterList[ptID - 1].size();
		for (int j = 0; j < clNum; j++) {
			/*
			 * Assign this point to the cluster. The clusterID starts from 1.
			 */
			assignPtToCluster(ptList[i], clusterIDMapping[eqList[ptClusterList[ptID - 1][j] - 1]]);
		}
	}
}

