/*
 * ApprDBSCAN.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: jhgan
 */

#include "ApprDBSCAN.h"

struct CellElement {
	GridCell* g;
	double sqr_dist;

	CellElement(GridCell* g, double sqr_dist) {
		this->g = g;
		this->sqr_dist = sqr_dist;
	}

	CellElement() {
		this->g = NULL;
		this->sqr_dist = 0;
	}

	/*
	 *  In order to use a min heap.
	 */
	bool operator <(const CellElement& g2) const {
		return this->sqr_dist > g2.sqr_dist;
	}
};

/*
 *  The compare function for elements.
 */
bool compCellElement(const CellElement& e1, const CellElement& e2) {
	return (e1.sqr_dist) < (e2.sqr_dist);
}

ApprDBSCAN::ApprDBSCAN(vector<Point*>& ptList) :
		Framework() {

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
}

ApprDBSCAN::~ApprDBSCAN() {
	// TODO Auto-generated destructor stub
}

vector<Point*>& ApprDBSCAN::getPtList() {
	return this->ptList;
}

int ApprDBSCAN::apprCounting_BestFirst(Point* q, vector<GridCell*>& neighborList) {
	/*
	 *  This is the heap for maintaining the "best" cell to the query point.
	 */
	priority_queue<CellElement> heap;
	const int nNum = neighborList.size();
	double dist = 0;
	double sqr_r = R * R;
	for (int i = 0; i < nNum; i++) {
		CellElement node(neighborList[i], sqrDistFromPointToCell(q, neighborList[i]));
		heap.push(node);
	}

	int cnt = 0;

	int state = 0;
	double distThreshold = R * (1 + PHI);
	distThreshold = distThreshold * distThreshold;

	while (!heap.empty() && cnt < K) {
		CellElement cur = heap.top();
		heap.pop();

		if (cur.g->getLevel() == LOWEST_LEVEL) {
			cnt += cur.g->getSize();
		} else {

			state = cur.g->stateWithSphere(q);
			if (state == 1) {
				// Fully inside.
				cnt += cur.g->getSize();
			} else if (state == -1) {
				// Fully outside.
				// Since this is the closest cell to point q and it is already fully
				// outside, so there is no need to check the rest cells.
				return cnt;
			} else if (cur.g->isLeaf) {
				// If the given grid cell is a leaf, then use brute-force method.

				vector<Point*>& ptList = cur.g->getPtList();
				int num = ptList.size();

				for (int i = 0; i < num; i++) {
					if (q->getSqrDist(ptList[i]) <= distThreshold) {
						cnt++;
						if (cnt >= K)
							break;
					}
				}
			} else {
				// Push all the sub core cells into heap.
				HashTable::iterator it = cur.g->gridTable->begin();
				while (it != cur.g->gridTable->end()) {
					dist = sqrDistFromPointToCell(q, it->second);
					if (dist <= sqr_r) {
						CellElement temp(it->second, dist);
						heap.push(temp);
					}
					it++;
				}
			}
		}
	}
	return cnt;
}

void ApprDBSCAN::identifyCorePts_ApprCounting() {
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it = table->begin();
	int ptNum = 0, cnt = 0;

	while (it != table->end()) {
		ptNum = it->second->getSize();
		if (ptNum >= K) {
			for (int i = 0; i < ptNum; i++) {
				it->second->markCore(i);
			}
		} else {
			vector<Point*>& ptList = it->second->getPtList();
			vector<GridCell*> neighborList;
			it->second->getNeighboringCells(neighborList);

			for (int i = 0; i < ptNum; i++) {
				cnt = apprCounting_BestFirst(ptList[i], neighborList);
				if (cnt >= K) {
					it->second->markCore(i);
				}
			}
		}
		it++;
	}

}

void ApprDBSCAN::identifyCorePts() {
	if (USE_NK) {
		identifyCorePts_nk((this->root->gridTable), R, K);
	} else {
		this->identifyCorePts_ApprCounting();
	}
}

/*
 *  Check whether there is a core point in the given grid cell within distance (1+\phi)r to query point q.
 *  If YES, return TRUE. Otherwise, return FALSE.
 *  Parameter List:
 *  	q:				the center point of the query sphere
 *  	g:				the pointer of the given grid cell
 *  	lowestLevel: 	the level of the grid cells with the smallest side length
 *  	phi:			the approximate parameter
 *  Return:
 *  	If the desired core point exists, return TRUE. Otherwise, return FALSE.
 */
bool nonEmptinessOnCore(Point* q, GridCell* g, int lowestLevel, double phi) {
	/*
	 *  This is the heap for maintaining the "best" cell to the query point.
	 */
	priority_queue<CellElement> heap;
	CellElement node(g, sqrDistFromPointToCell(q, g));
	heap.push(node);

	int level = 0;
	int coreNum = 0;
	int state = -1;
	double dist = 0;
	double sqr_r = R * R;
	double distThreshold = R * (1 + phi);
	distThreshold = distThreshold * distThreshold;

	while (!heap.empty()) {
		CellElement cur = heap.top();
		heap.pop();

		level = cur.g->getLevel();
		coreNum = cur.g->getCoreNum();

		if (coreNum > 0) {
			if (level == lowestLevel)
				return true;

			state = cur.g->stateWithSphere(q);
			if (state == 1) {
				// Fully inside.
				return true;
			} else if (state == -1) {
				// Fully outside.
				// Since this is the closest cell to point q and it is already fully
				// outside, so there is no need to check the rest cells.
				return false;
			}

			if (cur.g->isLeaf) {
				// If the given grid cell is a leaf, then use brute-force method.
				vector<Point*>& ptList = cur.g->getPtList();
				int num = cur.g->getCoreNum();

				for (int i = 0; i < num; i++) {
					if (q->getSqrDist(ptList[i]) <= distThreshold) {
						return true;
					}
				}
			} else {
				// Push all the sub core cells into heap.
				HashTable::iterator it = cur.g->gridTable->begin();
				while (it != cur.g->gridTable->end()) {
					if (it->second->getCoreNum() > 0) {
						dist = sqrDistFromPointToCell(q, it->second);
						if (dist <= sqr_r) {
							CellElement temp(it->second, dist);
							heap.push(temp);
						}
					}
					it++;
				}
			}
		}
	}

	return false;
}

struct PointElement {
	Point* pt;
	double sqr_dist;

	PointElement() {
		pt = NULL;
		sqr_dist = -1;
	}

	PointElement(Point* pt, double sqr_dist) {
		this->pt = pt;
		this->sqr_dist = sqr_dist;
	}

	bool operator <(const PointElement& g2) {
		return this->sqr_dist < g2.sqr_dist;
	}
};

/*
 *  The compare function for elements.
 */
bool compPointElement2(const PointElement& e1, const PointElement& e2) {
	return (e1.sqr_dist) < (e2.sqr_dist);
}

/*
 *  This is latest version before trying to improve.
 */
bool edgeAddable_Appr_Recursive(GridCell* g1, GridCell* g2, double r) {

	int coreNum1 = g1->getCoreNum();
	int coreNum2 = g2->getCoreNum();

	if (coreNum1 == 0 || coreNum2 == 0) {
		return false;
	}

	if (coreNum1 <= coreNum2) {
		vector<Point*>& corePtList = g1->getPtList();
		for (int i = 0; i < coreNum1; i++) {
			if (nonEmptinessOnCore(corePtList[i], g2, LOWEST_LEVEL, PHI)) {
				return true;
			}
		}
	} else {
		vector<Point*>& corePtList = g2->getPtList();
		for (int i = 0; i < coreNum2; i++) {
			if (nonEmptinessOnCore(corePtList[i], g1, LOWEST_LEVEL, PHI)) {
				return true;
			}
		}
	}

	return false;
}

bool edgeAddable_Appr(GridCell* g1, GridCell* g2, double r) {
	const int coreNum1 = g1->getCoreNum();
	const int coreNum2 = g2->getCoreNum();

	if (coreNum1 == 0 || coreNum2 == 0) {
		return false;
	}

	double start = 0;
	double end = 0;

	bool rtn = false;
	if (coreNum1 < 1000 || coreNum2 < 1000) {
		start = getCurrentTime();
		rtn = edgeAddable_Appr_Recursive(g1, g2, r);
		end = getCurrentTime();
		PreSORTINGTIME += end - start;
		return rtn;
//		return edgeAddable_Appr_Recursive(g1, g2, r);
	}

	start = getCurrentTime();

	int lowestLevel = LOWEST_LEVEL;
	double phi = PHI;

	vector<Point*>& coreList1 = g1->getPtList();
	vector<Point*>& coreList2 = g2->getPtList();

	double sqr_r = r * r;

	// Compute the closest distances from each core point to g2 and sort by them.

	PointElement* eleList1 = new PointElement[coreNum1];
	PointElement* eleList2 = new PointElement[coreNum2];
	int cnt1 = 0;
	int cnt2 = 0;

	double sqr_dist = 0;
	for (int i = 0; i < coreNum1; i++) {
		sqr_dist = sqrDistFromPointToCell(coreList1[i], g2);
		if (sqr_dist <= sqr_r) {
			eleList1[cnt1].pt = coreList1[i];
			eleList1[cnt1].sqr_dist = sqr_dist;
			cnt1++;
		}
	}

	for (int i = 0; i < coreNum2; i++) {
		sqr_dist = sqrDistFromPointToCell(coreList2[i], g1);
		if (sqr_dist <= sqr_r) {
			eleList2[cnt2].pt = coreList2[i];
			eleList2[cnt2].sqr_dist = sqr_dist;
			cnt2++;

		}
	}

	end = getCurrentTime();
//	PreSORTINGTIME += end - start;

	start = getCurrentTime();

	if (cnt1 > 1) {
		sort(eleList1, eleList1 + cnt1, compPointElement2);
	}
	if (cnt2 > 1) {
		sort(eleList2, eleList2 + cnt2, compPointElement2);
	}

	end = getCurrentTime();
	SORTINGTIME += end - start;

	// Start to find the bichromatic closest pair.
	int pos1 = 0;
	int pos2 = 0;
	while (pos1 < cnt1 && pos2 < cnt2) {
		if (eleList1[pos1].sqr_dist <= eleList2[pos2].sqr_dist) {
			if (nonEmptinessOnCore(eleList1[pos1].pt, g2, lowestLevel, phi)) {
				delete[] eleList1;
				delete[] eleList2;
				return true;
			}
			pos1++;
		} else {
			if (nonEmptinessOnCore(eleList2[pos2].pt, g1, lowestLevel, phi)) {
				delete[] eleList1;
				delete[] eleList2;
				return true;
			}
			pos2++;
		}
	}

	delete[] eleList1;
	delete[] eleList2;
	return false;
}

bool edgeAddable_Appr_Sort(GridCell* g1, GridCell* g2, double r) {
	double sqr_dist = 0;
	double sqr_r = r * r;

	if (g1->getCoreNum() == 0 || g2->getCoreNum() == 0) {
		return false;
	}

	double start = 0;
	double end = 0;

	if (g1->isLeaf && g2->isLeaf) {

		start = getCurrentTime();

		int ptNum1 = g1->getCoreNum();
		int ptNum2 = g2->getCoreNum();
		vector<Point*>& ptList1 = g1->getPtList();
		vector<Point*>& ptList2 = g2->getPtList();
		int lowestLevel = LOWEST_LEVEL;
		double phi = PHI;
		double distThreshold = (1 + phi) * (1 + phi) * sqr_r;

		if (g1->getLevel() == lowestLevel && g2->getLevel() == lowestLevel) {
			// Both g1 and g2 are at the lowest level.
			if (ptNum1 > ptNum2) {
				for (int i = 0; i < ptNum2; i++) {
					if (g1->stateWithSphere(ptList2[i]) != -1) {
						return true;
					}
				}
			} else {
				for (int i = 0; i < ptNum1; i++) {
					if (g2->stateWithSphere(ptList1[i]) != -1) {
						return true;
					}
				}
			}
		} else if (g1->getLevel() == lowestLevel) {
			// Only g1 is at the lowest level.
			for (int i = 0; i < ptNum2; i++) {
				if (g1->stateWithSphere(ptList2[i]) != -1) {
					return true;
				}
			}
		} else if (g2->getLevel() == lowestLevel) {
			// Only g2 is at the lowest level.
			for (int i = 0; i < ptNum1; i++) {
				if (g2->stateWithSphere(ptList1[i]) != -1) {
					return true;
				}
			}
		} else {
			// Both g1 and g2 are not at the lowest level. However, in this case, the number
			// of points in g1 and g2 is small.

			int state = -1;

			if (ptNum1 <= ptNum2) {
				for (int i = 0; i < ptNum1; i++) {
					state = g2->stateWithSphere(ptList1[i]);
					if (state == 1) {
						// Fully inside.
						return true;
					} else if (state == 0) {
						// Intersect.
						for (int j = 0; j < ptNum2; j++) {
							if (ptList1[i]->getSqrDist(ptList2[j]) <= distThreshold) {
								return true;
							}
						}
					}

				}

			} else {
				for (int i = 0; i < ptNum2; i++) {
					state = g1->stateWithSphere(ptList2[i]);
					if (state == 1) {
						// Fully inside.
						return true;
					} else if (state == 0) {
						// Intersect.
						for (int j = 0; j < ptNum1; j++) {
							if (ptList2[i]->getSqrDist(ptList1[j]) <= distThreshold) {
								return true;
							}
						}
					}

				}
			}
		}
		end = getCurrentTime();
		SORTINGTIME += end - start;
		return false;
	}

	start = getCurrentTime();

	int num1 = 0;
	CellElement* cellList1 = NULL;
	int cnt1 = 0;

	if (!g1->isLeaf) {
		num1 = g1->gridTable->size();
		cellList1 = new CellElement[num1];
		cnt1 = 0;
		HashTable::iterator it1 = g1->gridTable->begin();
		while (it1 != g1->gridTable->end()) {
			if (it1->second->getCoreNum() > 0) {
				sqr_dist = sqrDistBetweenCells(it1->second, g2);
				if (sqr_dist <= sqr_r) {
					cellList1[cnt1].g = it1->second;
					cellList1[cnt1].sqr_dist = sqr_dist;
					cnt1++;
				}
			}
			it1++;
		}
		if (cnt1 > 0) {
			sort(cellList1, cellList1 + cnt1, compCellElement);
		}
	} else {
		sqr_dist = sqrDistBetweenCells(g1, g2);
		if (sqr_dist <= sqr_r) {
			num1 = 1;
			cellList1 = new CellElement[num1];
			cellList1[cnt1].g = g1;
			cellList1[cnt1].sqr_dist = sqr_dist;
			cnt1++;
		} else {
			return false;
		}
	}

	int num2 = 0;
	CellElement* cellList2 = NULL;
	int cnt2 = 0;

	if (!g2->isLeaf) {
		num2 = g2->gridTable->size();
		cellList2 = new CellElement[num2];
		cnt2 = 0;
		HashTable::iterator it2 = g2->gridTable->begin();
		while (it2 != g2->gridTable->end()) {
			if (it2->second->getCoreNum() > 0) {
				sqr_dist = sqrDistBetweenCells(it2->second, g1);
				if (sqr_dist <= sqr_r) {
					cellList2[cnt2].g = it2->second;
					cellList2[cnt2].sqr_dist = sqr_dist;
					cnt2++;
				}
			}
			it2++;
		}
		if (cnt2 > 0) {
			sort(cellList2, cellList2 + cnt2, compCellElement);
		}
	} else {
		sqr_dist = sqrDistBetweenCells(g1, g2);
		if (sqr_dist <= sqr_r) {
			num2 = 1;
			cellList2 = new CellElement[num2];
			cellList2[cnt2].g = g2;
			cellList2[cnt2].sqr_dist = sqr_dist;
			cnt2++;
		} else {
			return false;
		}
	}

//	SORTINGTIME += (clock() - start) / (double) CLOCKS_PER_SEC;

	int pos1 = 0;
	int pos2 = 0;

	while (pos1 < cnt1 && pos2 < cnt2) {

		if (cellList1[pos1].sqr_dist <= cellList2[pos2].sqr_dist) {
			for (int i = pos2; i < cnt2; i++) {
				if (edgeAddable_Appr_Sort(cellList1[pos1].g, cellList2[i].g, r)) {
					delete[] cellList1;
					delete[] cellList2;
					end = getCurrentTime();
					PreSORTINGTIME += end - start;
					return true;
				}
			}
			pos1++;
		} else {
			for (int i = pos1; i < cnt1; i++) {
				if (edgeAddable_Appr_Sort(cellList1[i].g, cellList2[pos2].g, r)) {
					delete[] cellList1;
					delete[] cellList2;
					end = getCurrentTime();
					PreSORTINGTIME += end - start;
					return true;
				}
			}
			pos2++;
		}
	}

	delete[] cellList1;
	delete[] cellList2;
	end = getCurrentTime();
	PreSORTINGTIME += end - start;
	return false;
}

void ApprDBSCAN::constructGridGraph() {
	double start = 0, end = 0, duration = 0;
	start = getCurrentTime();
	// Construct our data structure for approximate counting.
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it = table->begin();
	while (it != table->end()) {
		it->second->split(LOWEST_LEVEL);
		it++;
	}
	end = getCurrentTime();
	duration = end - start;
	printf("Cell splitting: %lf second.\n", duration);
	fflush(stdout);

//((Framework*) this)->constructGridGraph(&edgeAddable_Appr);
	if (USE_PROGRESSIVE) {
		this->constructGridGraph_Progressive(&edgeExists_Progressive, &edgeAddable_Appr_Recursive);
	} else if (USE_SPACE_PRIOR) {
		((Framework*) this)->constructGridGraph_SpacePrior(&edgeAddable_Appr_Recursive);
	} else {
//		((Framework*) this)->constructGridGraph(&edgeAddable_Appr);
//		((Framework*) this)->constructGridGraph(&edgeAddable_Appr_Sort);
		((Framework*) this)->constructGridGraph(&edgeAddable_Appr_Recursive);
	}
}

void ApprDBSCAN::clusterCorePts() {
	if ((!USE_PROGRESSIVE) && (!USE_SPACE_PRIOR)) {
		((Framework*) this)->clusterCorePts_Org();
	}
}

/*
 *  Check whether the given non-core point can be assigned to the given grid cell.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool assignable(Point* p, GridCell* g, double r) {
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

void ApprDBSCAN::assignNonCorePts() {
	HashTable* table = (this->root->gridTable);
	HashTable::iterator it = table->begin();

	int neighborNum = 0;
	int startIndex = 0;
	int ptNum = 0;

//	double start = 0;
//	double end = 0;
//	double duration = 0, duration2;

	while (it != table->end()) {
		startIndex = it->second->getCoreNum();
		ptNum = it->second->getSize();

		if (startIndex < ptNum) { // there is at least a non-core point
			vector<Point*>& ptList = it->second->getPtList();

//			start = getCurrentTime();

			vector<GridCell*> neighborList;
			it->second->getNeighboringCells(neighborList);
			neighborNum = neighborList.size();

//			end = getCurrentTime();
//			duration += end - start;
//
//			start = getCurrentTime();

			// enumerate each non-core point
			for (int i = startIndex; i < ptNum; i++) {
				for (int j = 0; j < neighborNum; j++) {
					if (it->second != neighborList[j] && neighborList[j]->getCoreNum() > 0) {

						if (assignable(ptList[i], neighborList[j], R)) {
							/*
							 *  Assign this point to the same cluster of the core points.
							 */
							assignPtToCluster(ptList[i],
									neighborList[j]->getPtList()[0]->clusterID);

						}

					} else if (startIndex > 0) {
						// There is at least a core point in this cell.
						/*
						 *  Assign this point to the same cluster of the core points.
						 */
						assignPtToCluster(ptList[i], ptList[0]->clusterID);

					}
				}
			}
//			end = getCurrentTime();
//			duration2 += end - start;
		}
		it++;
	}

//	printf("Time for getting neighbor list: %lf second.\n", duration);
//	printf("Time for computing edge: %lf second.\n", duration2);
//	fflush(stdout);
}
