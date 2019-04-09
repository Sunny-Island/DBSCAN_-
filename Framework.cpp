/*
 * Framework.cpp
 *
 *  Created on: Sep 4, 2014
 *      Author: jhgan
 */

#include "Framework.h"

Framework::Framework() {
	this->root = new GridCell();
	GridCell::objectCnt++;

	GridCell::setRoot(this->root);
	this->clusterCnt = 0;
}

Framework::~Framework() {
	this->releaseSpace();
}

void Framework::releaseSpace() {
	if (root != NULL) {
		root->bst.clear();
		root->releaseSpace();
		delete root;
		root = NULL;
		GridCell::objectCnt--;
	}
}

int Framework::getClusterCnt() {
	return this->clusterCnt;
}

void Framework::clustering() {

	double start = 0;
	double end = 0;
	double duration = 0;

	start = getCurrentTime();
	this->identifyCorePts();
	end = getCurrentTime();
	duration = end - start;
	printf("identify core point: %lf second.\n", duration);
	fflush(stdout);

	start = getCurrentTime();
	this->constructGridGraph();
	end = getCurrentTime();
	duration = end - start;
	printf("construct graph: %lf second.\n", duration);
	fflush(stdout);

	start = getCurrentTime();
	this->clusterCorePts();
	end = getCurrentTime();
	duration = end - start;
	printf("cluster core points: %lf second.\n", duration);
	fflush(stdout);

	start = getCurrentTime();
	this->assignNonCorePts();
	end = getCurrentTime();
	duration = end - start;
	printf("assign non-core point: %lf second.\n", duration);
	fflush(stdout);
}

void Framework::constructGridGraph_SpacePrior(bool (*edgeAddable)(GridCell*, GridCell*, double)) {

	// Here we don't need to initialize the adjacent lists because we construct the grid graph *IMPLICITLY*.
	// However, each edge will be computed twice.

	HashTable* table = (root->gridTable);
	HashTable::iterator it = table->begin();

	double r = R;
	int neighborNum = 0;
	bool isDone = false;

	/*** Test for running time ***/
	double start = 0;
	double end = 0;
	double duration = 0;
	int checkNum = 0;
	int edgeNum = 0;

	double neighborTime = 0;

	queue<GridCell*> que;
	GridCell* cur = NULL;
	int clusterID = 1;

	vector<GridCell*> neighborList;
	while (it != table->end()) {
		if (it->second->isProcessed == false && it->second->getCoreNum() > 0) {

			que.push(it->second);
			it->second->isProcessed = true;

			while (!que.empty()) {
				cur = que.front();
				que.pop();

				// Assign all the core point in cur to the current cluster.
				assignCorePtsToCluster(cur, clusterID);

				start = getCurrentTime();

				neighborList.clear();
				cur->getNeighboringCells(neighborList);
				neighborNum = neighborList.size();

				end = getCurrentTime();
				neighborTime += end - start;

				for (int i = 0; i < neighborNum; i++) {
					isDone = false;
					if (neighborList[i]->getCoreNum() > 0) {

						start = getCurrentTime();

						isDone = edgeAddable(cur, neighborList[i], r);

						end = getCurrentTime();
						duration += end - start;

						/***** For checking the edge checking ********/
						checkNum++;
						/*********************************************/

						if (isDone) {
							edgeNum++;
							if (neighborList[i]->isProcessed == false) {
								que.push(neighborList[i]);
								neighborList[i]->isProcessed = true;
							}
						}
					}
				}
			}
			// When this while loop ends, a new cluster has been found.
			clusterID++;
			this->clusterCnt++;
		}
		it++;
	}

	printf("==================================================================\n");
	printf("There are %d edges checking.\n", checkNum);
	printf("There are %d edges added.\n", edgeNum);
	printf("The time for checking edge: %lf second.\n", duration);
	printf("The time for get neighbor list: %lf second.\n", neighborTime);
	printf("==================================================================\n");
	fflush(stdout);
}

void Framework::constructGridGraph(bool (*edgeExists)(GridCell*, GridCell*),
		bool (*edgeAddable)(GridCell*, GridCell*, double)) {

	// Initialize the adjacent lists for the core cells.
	root->initializeAdjacentLists();

	HashTable* table = (root->gridTable);
	HashTable::iterator it = table->begin();

	double r = R;
	int neighborNum = 0;
	bool isDone = false;

	/***Test for running time**/
	double start = 0;
	double end = 0;
	double duration = 0;
	int checkNum = 0;
	int edgeNum = 0;
//	SORTINGTIME = 0;
//	PreSORTINGTIME = 0;

	double existTime = 0;
	double addingTime = 0;
	double neighborTime = 0;

	vector<GridCell*> neighborList;
	while (it != table->end()) {

		if (it->second->getCoreNum() > 0) {

			start = getCurrentTime();

			neighborList.clear();
			it->second->getNeighboringCells(neighborList);
			neighborNum = neighborList.size();

			end = getCurrentTime();
			neighborTime += end - start;

			for (int i = 0; i < neighborNum; i++) {
				isDone = false;
				if (it->second != neighborList[i] && neighborList[i]->getCoreNum() > 0) {

					start = getCurrentTime();

					// check whether there already exists an edge between these two grid cells
					isDone = edgeExists(it->second, neighborList[i]);

					end = getCurrentTime();
					existTime += end - start;

					if (!isDone) {

						start = getCurrentTime();

						isDone = edgeAddable(it->second, neighborList[i], r);

						end = getCurrentTime();
						duration += end - start;

						/***** For checking the edge checking*********/
						checkNum++;
						/*************************/

						if (isDone) {
							edgeNum++;
							start = getCurrentTime();
							// Add an edge between these two grid cells.
							it->second->addAdjacentCell(neighborList[i]);
							neighborList[i]->addAdjacentCell(it->second);

							end = getCurrentTime();
							addingTime += end - start;
						}
					}
				}
			}
		}
		it++;
	}

	printf("==================================================================\n");
	printf("There are %d edges checking.\n", checkNum);
	printf("There are %d edges added.\n", edgeNum);
	printf("The time for checking edge: %lf second.\n", duration);
	printf("The time for existence: %lf second.\n", existTime);
	printf("The time for adding to set: %lf second.\n", addingTime);
	printf("The time for get neighbor list: %lf second.\n", neighborTime);
//	printf("The time for sorting: %lf second.\n", SORTINGTIME);
//	printf("The time before sorting: %lf second.\n", PreSORTINGTIME);
	printf("==================================================================\n");
	fflush(stdout);

}

/*
 *  Check whether there exists an edge between the two given grid cells by
 *  the set of adjacent IDs.
 *  If the edge already exists, return TRUE. Otherwise, return FALSE.
 */
bool edgeExists_Set(GridCell* g1, GridCell* g2) {
	if (g1->adjacentList->size() > g2->adjacentList->size()) {
		return g2->adjacentList->find(g1) != g2->adjacentList->end();
	} else {
		return g1->adjacentList->find(g2) != g1->adjacentList->end();
	}
}

void Framework::constructGridGraph(bool (*edgeAddable)(GridCell*, GridCell*, double)) {
	this->constructGridGraph(&edgeExists_Set, edgeAddable);
}

void Framework::constructGridGraph_Progressive(bool (*edgeExists)(GridCell*, GridCell*),
		bool (*edgeAddable)(GridCell*, GridCell*, double)) {

	// Here we don't need to initialize the adjacent lists because we construct the grid graph *IMPLICITLY*.
	// The only difference between the "SpacePrior" version is that this version greatly reduces the number of edge checks
	// by directly computing the connected component.

	HashTable* table = (root->gridTable);
	HashTable::iterator it = table->begin();

	double r = R;

	// Clear the record lists in ToolFunctions.cpp.
	clearLists();

	int cnt = 0;

	int clusterID = 1;

	// expand connected component to construct grid graph.
	while (it != table->end()) {
		if (it->second->isProcessed == false && it->second->getCoreNum() > 0) {
			cnt += expandFromGridCell(it->second, r, edgeExists, edgeAddable, clusterID);
//			curComponent.clear();
			clusterID++;
			this->clusterCnt++;
		}
		it++;
	}

	/**********For edge checking***********/
	printf("==================================================================\n");
	printf("There are %d edge checking.\n", cnt);
	printf("==================================================================\n");
	fflush(stdout);
	/**************************************/

	// Clear the record lists in ToolFunctions.cpp.
	clearLists();
}

void Framework::expandConnectedComponet(GridCell* g, int clusterID) {
	queue<GridCell*> que;
	que.push(g);
	g->isProcessed = true;

	set<GridCell*, GridCell::GridCellIDComp>::iterator it;
	GridCell* cur = NULL;
	while (!que.empty()) {
		cur = que.front();
		que.pop();

		assignCorePtsToCluster(cur, clusterID);

		it = cur->adjacentList->begin();
		while (it != cur->adjacentList->end()) {
			if ((*it)->isProcessed == false) {
				que.push(*it);
				(*it)->isProcessed = true;
			}
			it++;
		}
		/*
		 *  Free the memory of the adjacentList of cur.
		 */
		cur->adjacentList->clear();
		delete (cur->adjacentList);
		cur->adjacentList = NULL;
	}
}

void Framework::clusterCorePts_Org() {
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it = table->begin();
	int clusterID = 1;
	while (it != table->end()) {
		if (it->second->isProcessed == false && it->second->getCoreNum() > 0) {
			expandConnectedComponet(it->second, clusterID);
			clusterID++;
			this->clusterCnt++;
		}
		it++;
	}
}

