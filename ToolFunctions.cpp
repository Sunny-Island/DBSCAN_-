/*
 * ToolFunctions.cpp
 *
 *  Created on: Sep 23, 2014
 *      Author: jhgan
 */

#include "ToolFunctions.h"

/*
 ******************************************************************
 *  The following functions are for Gunawan Exact DBSCAN
 ******************************************************************
 */

/*
 *  Check whether it can be added an edge between the two given grid cells.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeAddable_Exact(GridCell* p1, GridCell* p2, double r) {
	int coreNum = p1->getCoreNum();
	int num = p2->getCoreNum();
	if (coreNum == 0 || num == 0) {
		return false;
	}
	vector<Point*>& coreList = p1->getPtList();
	vector<Point*>& neighborCoreList = p2->getPtList();
	double sqr_r = r * r;

	for (int j = 0; j < coreNum; j++) {
		for (int l = 0; l < num; l++) {
			if (coreList[j]->getSqrDist(neighborCoreList[l]) <= sqr_r) {
				return true;
			}
		}
	}
	return false;
}

/*
 *  Check whether there is an edge between the two given grid cells by Nearest Neighbor query.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeAddable_Exact_NN(GridCell* g1, GridCell* g2, double r) {
	int coreNum1 = g1->getCoreNum();
	int coreNum2 = g2->getCoreNum();
	if (coreNum1 == 0 || coreNum2 == 0) {
		return false;
	}

	vector<Point*>& coreList1 = g1->getPtList();
	vector<Point*>& coreList2 = g2->getPtList();

	double minSqrDist = -1;
	double sqrDist = 0;
	const double sqr_r = r * r;

	if (coreNum1 <= coreNum2) {
		for (int i = 0; i < coreNum1; i++) {
			minSqrDist = coreList1[i]->getSqrDist(coreList2[0]);
			for (int j = 1; j < coreNum2; j++) {
				sqrDist = coreList1[i]->getSqrDist(coreList2[j]);
				if (sqrDist < minSqrDist) {
					minSqrDist = sqrDist;
				}
			}
			if (minSqrDist <= sqr_r) {
				return true;
			}
		}
	} else {
		for (int i = 0; i < coreNum2; i++) {
			minSqrDist = coreList2[i]->getSqrDist(coreList1[0]);
			for (int j = 1; j < coreNum1; j++) {
				sqrDist = coreList2[i]->getSqrDist(coreList1[j]);
				if (sqrDist < minSqrDist) {
					minSqrDist = sqrDist;
				}
			}
			if (minSqrDist <= sqr_r) {
				return true;
			}
		}
	}

	return false;
}

/*****************************************************************/

/*
 ******************************************************************
 *  The following functions are for our Exact DBSCAN
 ******************************************************************
 */

/*
 *  The element for sorting by the distance to closest corner.
 */
struct PointElement {
	Point* pt;
	double sqr_dist; // the square of distance

	PointElement(Point* pt, double sqr_dist) {
		this->pt = pt;
		this->sqr_dist = sqr_dist;
	}

	PointElement() {
		this->pt = NULL;
		this->sqr_dist = 0;
	}
};

/*
 *  The compare function for elements.
 */
bool compPointElement(const PointElement& e1, const PointElement& e2) {
	return (e1.sqr_dist) < (e2.sqr_dist);
}

/*
 *  Compute the square of the minimum distance from a given point to a given cell.
 *  Parameter List:
 *  	p:		the given point
 *  	g:		the given cell
 *  Return:
 *  	The square of the minimum distance from p to g.
 */
double sqrDistFromPointToCell(Point* p, GridCell* g) {
	double sqr_dist = 0;
	double temp1 = 0;
	double temp2 = 0;

	int dim = DIM;
	vector<int>& gridCoords = g->getCoords();
	double sideLen = g->getSideLength();

	// Compute the closest distance from this point to g2.
	for (int j = 0; j < dim; j++) {
		temp1 = gridCoords[j] * sideLen - p->coords[j];
		temp2 = temp1 + sideLen;
		if (temp1 > 0) {
			sqr_dist += temp1 * temp1;
		} else if (temp2 < 0) {
			sqr_dist += temp2 * temp2;
		}
	}
	return sqr_dist;
}

/*
 *  Compute the square of the minimum distance between the two given cells. Note that the two cells are
 *  not necessarily at the same level.
 */
double sqrDistBetweenCells(GridCell* g1, GridCell* g2) {
	double sqr_dist = 0;
	int dim = DIM;
	double sideLen1 = g1->getSideLength();
	double sideLen2 = g2->getSideLength();
	vector<int>& coords1 = g1->getCoords();
	vector<int>& coords2 = g2->getCoords();
	double temp = 0;

	for (int i = 0; i < dim; i++) {
		if (coords1[i] * sideLen1 > (coords2[i] + 1) * sideLen2) {
			temp = coords1[i] * sideLen1 - (coords2[i] + 1) * sideLen2;
			sqr_dist += temp * temp;
		} else if ((coords1[i] + 1) * sideLen1 < coords2[i] * sideLen2) {
			temp = coords2[i] * sideLen2 - (coords1[i] + 1) * sideLen1;
			sqr_dist += temp * temp;
		}
	}
	return sqr_dist;
}

/*
 *  Compute the square of the maximum distance between the two given cells. Note that the two cells are
 *  not necessarily at the same level.
 */
double sqrMaxDistBetweenCells(GridCell* g1, GridCell* g2) {
	double sqr_dist = 0;
	int dim = DIM;
	double sideLen1 = g1->getSideLength();
	double sideLen2 = g2->getSideLength();
	vector<int>& coords1 = g1->getCoords();
	vector<int>& coords2 = g2->getCoords();
	double temp1 = 0;
	double temp2 = 0;
	double x1 = 0;
	double x2 = 0;

	for (int i = 0; i < dim; i++) {
		x1 = coords1[i] * sideLen1;
		x2 = coords2[i] * sideLen2;
		temp1 = sideLen2 + x2 - x1;
		temp1 *= temp1;

		temp2 = sideLen1 + x1 - x2;
		temp2 *= temp2;

		if (temp1 > temp2) {
			sqr_dist += temp1;
		} else {
			sqr_dist += temp2;
		}
	}

	return sqr_dist;
}

double SORTINGTIME;
double PreSORTINGTIME;

/*
 *  Check whether it can be added an edge between the two given grid cells.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeAddable_BCP(GridCell* g1, GridCell* g2, double r) {
	const int coreNum = g1->getCoreNum();
	const int num = g2->getCoreNum();
	if (coreNum == 0 || num == 0) {
		return false;
	}

	if (coreNum * num < 10000) {
		return edgeAddable_Exact(g1, g2, r);
	}

	double start = getCurrentTime();

	vector<Point*>& coreList = g1->getPtList();
	vector<Point*>& neighborCoreList = g2->getPtList();

	double sqr_r = r * r;

	// Compute the closest distances from each core point to g2 and sort by them.
	PointElement* eleList1 = new PointElement[coreNum];
	PointElement* eleList2 = new PointElement[num];
	int cnt1 = 0;
	int cnt2 = 0;

	double sqr_dist = 0;

	for (int i = 0; i < coreNum; i++) {
		sqr_dist = sqrDistFromPointToCell(coreList[i], g2);

		if (sqr_dist <= sqr_r) {
			// If the distance of a core point to c2 is already > r,
			// then the distance between its NN in g2 and this core point is also > r.

			eleList1[cnt1].pt = coreList[i];
			eleList1[cnt1].sqr_dist = sqr_dist;
			cnt1++;
		}
	}

	for (int i = 0; i < num; i++) {
		sqr_dist = sqrDistFromPointToCell(neighborCoreList[i], g1);

		if (sqr_dist <= sqr_r) {
			// If the distance of a core point to c2 is already > r,
			// then the distance between its NN in g2 and this core point is also > r.
//			eleList2.push_back(new PointElement(neighborCoreList[i], sqr_dist));

			eleList2[cnt2].pt = neighborCoreList[i];
			eleList2[cnt2].sqr_dist = sqr_dist;
			cnt2++;

		}
	}

	double end = getCurrentTime();
	PreSORTINGTIME += end - start;

	start = getCurrentTime();

	if (cnt1 > 1) {
		sort(eleList1, eleList1 + cnt1, compPointElement);
	}
	if (cnt2 > 1) {
		sort(eleList2, eleList2 + cnt2, compPointElement);
	}

	end = getCurrentTime();

	SORTINGTIME += end - start;

// Start to find the bichromatic closest pair.
	int pos1 = 0;
	int pos2 = 0;
	int num1 = cnt1;
	int num2 = cnt2;
	while (pos1 < num1 && pos2 < num2) {
		if (eleList1[pos1].sqr_dist <= eleList2[pos2].sqr_dist) {
			for (int i = pos2; i < num2; i++) {
				if (eleList1[pos1].pt->getSqrDist(eleList2[i].pt) <= sqr_r) {
					// Release the space after used.
					delete[] eleList1;
					delete[] eleList2;
					return true;
				}
			}
			pos1++;
		} else {
			for (int i = pos1; i < num1; i++) {
				if (eleList2[pos2].pt->getSqrDist(eleList1[i].pt) <= sqr_r) {
					// Release the space after used.
					delete[] eleList1;
					delete[] eleList2;
					return true;
				}
			}
			pos2++;
		}
	}
	// Release the space after used.
	delete[] eleList1;
	delete[] eleList2;
	return false;
}

void identifyCorePts_nk(HashTable* table, double r, int k) {
	double start = 0;
	double end = 0;
	double duration = 0;

	HashTable::iterator it;
	double sqr_r = r * r;
	int cnt = 0;
	int state = 1;

	vector<GridCell*> neighborList;
	int neighborNum = 0;
	Point* pt = NULL;
	for (it = table->begin(); it != table->end(); it++) {
		vector<Point*>& ptList = it->second->getPtList();
		int ptNum = ptList.size();

		if (ptNum >= k) {
			// There are no less than k points in this grid cell.
			// All the points in this grid cell are core points.
			for (int i = 0; i < ptNum; i++) {
				// Mark this point as a core point.
				it->second->markCore(i);
			}
		} else {
			start = getCurrentTime();

			neighborList.clear();
			it->second->getNeighboringCells(neighborList);
			neighborNum = neighborList.size();

			end = getCurrentTime();
			duration += end - start;
//			printf("The time for getting neighbor list of grid cell %d is %lf seconds.\n",
//					it->second->getID(), end - start);
//			fflush(stdout);

			pt = NULL;
			for (int i = 0; i < ptNum; i++) {
				cnt = ptNum;
				pt = ptList[i];
				for (int j = 0; j < neighborNum; j++) {
					if (it->second != neighborList[j]) {
						state = neighborList[j]->stateWithSphere(pt);
						if (state == 1) {
							// fully inside
							cnt += neighborList[j]->getSize();
						} else if (state == 0) {
							// intersects with the boundary
							vector<Point*>& neighborPtList = neighborList[j]->getPtList();
							int num = neighborPtList.size();
							for (int l = 0; l < num; l++) {
								if (pt->getSqrDist(neighborPtList[l]) <= sqr_r) {
									cnt++;
									// Jump out of the loop as soon as cnt >= k.
									if (cnt >= k)
										break;
								}
							}
						}
					}
					// Jump out of the loop as soon as cnt >= k.
					if (cnt >= k)
						break;
				}
				if (cnt >= k) {
					// There are no less than k points in the sphere of this point.
					// Mark this point as a core point.
					it->second->markCore(i);
				}
			}
		}
	}

	printf("The time for getting neighbor cells is %lf seconds.\n", duration);
	fflush(stdout);
}

/*****************************************************************/

/*
 ******************************************************************
 *  The following functions are some heuristic for constructing grid graph.
 ******************************************************************
 */

/*
 *  The list for recording which pair of grid cells has been processed.
 */
set<pair<GridCell*, GridCell*> > processedList;

void clearLists() {
	processedList.clear();
//	curComponent.clear();
}

void assignCorePtsToCluster(GridCell* g, int clusterID) {
	int coreNum = g->getCoreNum();
	vector<Point*>& coreList = g->getPtList();

	for (int i = 0; i < coreNum; i++) {
		/*
		 *  Assign this core point the cluster.
		 */
		assignPtToCluster(coreList[i], clusterID);
	}
}

/*
 *  Check whether the given pair of two grid cells has ever been processed.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeExists_Progressive(GridCell* g1, GridCell* g2) {
	pair<GridCell*, GridCell*> processedPair;
	if (g1->getID() < g2->getID()) {
		processedPair.first = g1;
		processedPair.second = g2;
	} else {
		processedPair.first = g2;
		processedPair.second = g1;
	}

	if (processedList.find(processedPair) != processedList.end()) {
		// The given pair of grid cells have been processed.
		return true;
	}

	return false;
}

int expandFromGridCell(GridCell* g, double r, bool (*edgeExists)(GridCell*, GridCell*),
		bool (*edgeAddable)(GridCell*, GridCell*, double), int clusterID) {

	queue<GridCell*> que;
	que.push(g);
	g->isProcessed = true;

	int checkCnt = 0;
	int num = 0;
	vector<GridCell*> neighborList;
	GridCell* cur = NULL;
	while (!que.empty()) {
		cur = que.front();
		que.pop();

		// Assign all the core points to the current cluster.
		assignCorePtsToCluster(cur, clusterID);

		neighborList.clear();
		cur->getNeighboringCells(neighborList);
		num = neighborList.size();

		for (int i = 0; i < num; i++) {
			if (neighborList[i]->isProcessed == false && neighborList[i]->getCoreNum() > 0
					&& edgeExists(cur, neighborList[i]) == false) {
				/********For checking the number of edge checking.*******/
				checkCnt++;
				/********************************************************/

				// If the pair of grid cells has not been processed, then process it.
				if (cur->getID() < neighborList[i]->getID()) {
					processedList.insert(pair<GridCell*, GridCell*>(cur, neighborList[i]));
				} else {
					processedList.insert(pair<GridCell*, GridCell*>(neighborList[i], cur));
				}

				if (edgeAddable(cur, neighborList[i], r)) {
					que.push(neighborList[i]);
					neighborList[i]->isProcessed = true;
				}
			}
		}

	}
	return checkCnt;
}
