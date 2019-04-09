/*
 * DBSCAN.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: jhgan
 */

#include "ExactDBSCAN.h"

ExactDBSCAN::ExactDBSCAN(vector<Point*>& ptList, bool (*edgeAddable)(GridCell*, GridCell*, double)) :
		Framework() {

	this->edgeAddable = edgeAddable;
	this->ptList.swap(ptList);

	double start = 0;
	double end = 0;
	double duration = 0;

	start = getCurrentTime();

	this->root->constructGridCells(this->ptList);

	end = getCurrentTime();
	duration = end - start;
	printf("construct grid cells: %lf second.\n", duration);
	fflush(stdout);

	/*
	 * Output for Debug
	 */
//	HashTable::iterator it = this->root->gridTable.begin();
//	while (it != this->root->gridTable.end()) {
//		it->second->showInfo();
//		it++;
//	}
	/***********************************************************/
}

ExactDBSCAN::~ExactDBSCAN() {
}

vector<Point*>& ExactDBSCAN::getPtList() {
	return this->ptList;
}

void ExactDBSCAN::identifyCorePts() {
	identifyCorePts_nk((this->root->gridTable), R, K);
	/*
	 *  Codes for debug.
	 */
//	printf("===========================================================\n");
//	set<GridCell*>::iterator it_set = this->root->bst.begin();
//	while (it_set != this->root->bst.end()) {
//		(*it_set)->showInfo();
//		it_set++;
//	}
//	printf("===========================================================\n");
	/***********************************************************/
}

void ExactDBSCAN::constructGridGraph() {
	if (USE_PROGRESSIVE) {
		this->constructGridGraph_Progressive(&edgeExists_Progressive, this->edgeAddable);
	} else if (USE_SPACE_PRIOR) {
		((Framework*) this)->constructGridGraph_SpacePrior(this->edgeAddable);
	} else {
		((Framework*) this)->constructGridGraph(this->edgeAddable);
	}
}

void ExactDBSCAN::clusterCorePts() {
	if (USE_PROGRESSIVE == false && USE_SPACE_PRIOR == false) {
		((Framework*) this)->clusterCorePts_Org();
	}
}

/*
 *  Check whether the given non-core point can be assigned to the given grid cell.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool ExactDBSCAN::assignable(Point* p, GridCell* g, double r) {
	// get the point list of the neighboring cell
	vector<Point*>& neighborPtList = g->getPtList();
	int num = g->getCoreNum();
	double sqr_r = r * r;

	for (int l = 0; l < num; l++) {
		// compute the distance from *pt* to every core point in this neighboring cell
		if (p->getSqrDist(neighborPtList[l]) <= sqr_r) {
			return true;
		}
	}
	return false;
}

void ExactDBSCAN::assignNonCorePts() {
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it;
	int startIndex = 0, ptNum = 0, neighborNum = 0;

//	double start = 0;
//	double end = 0;
//	double duration = 0, duration2;

	for (it = table->begin(); it != table->end(); it++) {
		vector<Point*>& ptList = it->second->getPtList();
		startIndex = it->second->getCoreNum();
		ptNum = ptList.size();

		if (startIndex < ptNum) {

//			start = getCurrentTime();

			vector<GridCell*> neighborList;
			it->second->getNeighboringCells(neighborList);

//			end = getCurrentTime();
//			duration += end - start;
//
//			start = getCurrentTime();
			neighborNum = neighborList.size();
			Point* pt = NULL;
			for (int i = startIndex; i < ptNum; i++) {
				// Enumerate all the non-core points in this grid cell.
				pt = ptList[i];
				for (int j = 0; j < neighborNum; j++) {
					if (it->second != neighborList[j] && neighborList[j]->getCoreNum() > 0) {
						if (assignable(pt, neighborList[j], R)) {

							/*
							 *  Assign this point to the same cluster of the core points.
							 */
							assignPtToCluster(pt, neighborList[j]->getPtList()[0]->clusterID);

						}
					} else if (startIndex > 0) {
						// If there exists a core point in this grid cell, then assign all the non-core points
						// to the same cluster of this core point.
//						pt->assignToCluster((ptList[0]->clusterIDs[0]));

						/*
						 *  Assign this point to the same cluster of the core points.
						 */
						assignPtToCluster(pt, ptList[0]->clusterID);

					}
				}
			}
//			end = getCurrentTime();
//			duration2 += end - start;
		}
	}

//	printf("Time for getting neighbor list: %lf second.\n", duration);
//	printf("Time for computing edge: %lf second.\n", duration2);
//	fflush(stdout);
}

