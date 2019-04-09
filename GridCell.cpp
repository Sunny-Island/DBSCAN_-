/*
 * GridCell.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: jhgan
 */

#include "GridCell.h"
#include <math.h>

int GridCell::objectCnt = 0;

/*
 *  The root.
 */
GridCell* GridCell::root = NULL;

// The binary search tree for space filling curve.
set<GridCell*, GridCell::GridCellLess> GridCell::bst;

GridCell::GridCell(vector<int>& gridCoords, int ID, int lev) {
//	this->gridTable = new hash_map<const vector<int>&, GridCell*, hash_func, key_equal>;
	this->gridTable = NULL;
	this->adjacentList = NULL;

	this->ID = ID;
	this->gridCoords_key.swap(gridCoords);
	this->isProcessed = false;
	this->level = lev;
	this->sideLen = R / ((sqrt(DIM) * (1 << (lev - 1))));
	this->size = 0;
	this->coreNum = 0;
	this->isLeaf = false;

	if (this->gridCoords_key.size() == (unsigned int) DIM) {
//		this->gridCoords_key.push_back(this->modPrime % N);
		this->gridCoords_key.push_back(
				this->computeModPrime(this->gridCoords_key));
	}
//	GridCell::objectCnt++;
}

GridCell::GridCell() {
	this->gridTable = NULL;
	this->adjacentList = NULL;

	this->ID = 0;
	this->isProcessed = false;
	this->level = 0;
	this->size = 0;
	this->coreNum = 0;
	this->sideLen = 2 * R / (sqrt(DIM));
	this->isLeaf = false;
}

GridCell::~GridCell() {
//	this->releaseSpace();
//	GridCell::objectCnt--;
}

void GridCell::initializeAdjacentLists() {
	HashTable* table = this->gridTable;
	if (table != NULL) {
		HashTable::iterator it = table->begin();
		while (it != table->end()) {
			if (it->second->getCoreNum() > 0) {
				// We only create adjacent list for the core cells.
				it->second->adjacentList = new set<GridCell*, GridCellIDComp>;
			}
			it++;
		}
	}
}

void GridCell::setRoot(GridCell* myRoot) {
	GridCell::root = myRoot;
}

void GridCell::releaseSpace() {
	if (this->adjacentList != NULL) {
		this->adjacentList->clear();
		delete (this->adjacentList);
		this->adjacentList = NULL;
	}

	if (this->gridTable != NULL) {
		HashTable::iterator it = this->gridTable->begin();
		int cnt = this->gridTable->size();
		vector<GridCell*> cellList;
		cellList.reserve(cnt);
		while (it != this->gridTable->end()) {
			it->second->releaseSpace();
			cellList.push_back(it->second);
			it++;
		}
		this->gridTable->clear();
		delete (this->gridTable);
		this->gridTable = NULL;

		for (int i = 0; i < cnt; i++) {
			delete (cellList[i]);
			cellList[i] = NULL;
			GridCell::objectCnt--;
		}
		cellList.clear();
	}
}

int GridCell::getID() {
	return this->ID;
}

int GridCell::getSize() {
	return this->size;
}

int GridCell::getLevel() {
	return this->level;
}

int GridCell::getCoreNum() {
	return this->coreNum;
}

double GridCell::getSideLength() {
	return this->sideLen;
}

vector<Point*>& GridCell::getPtList() {
	return this->ptList;
}

vector<int>& GridCell::getCoords() {
	return this->gridCoords_key;
}

void GridCell::setSideLength(double sideLen) {
	if (sideLen == 0) {
		showError("Error in GridCell::setSideLength: sideLen cannot be 0!\n");
	}
	this->sideLen = sideLen;
//	this->oneOverSideLen = 1.0 / sideLen;
}

void GridCell::showCoords() {
	int dim = DIM;
	printf("Grid Cell Coordinates:\t");
	for (int i = 0; i < dim; i++) {
		printf("%d\t", this->gridCoords_key[i]);
	}
	printf("\n");
}

void GridCell::showInfo() {
	int dim = DIM;
	int ptNum = this->size;
//	int ptNum = this->ptList.size();
	printf("\n**********************************************\n");
	printf("Grid Cell Coordinates:\t");
	for (int i = 0; i < dim; i++) {
		printf("%d\t", this->gridCoords_key[i]);
	}
	if ((int) (this->gridCoords_key.size()) >= dim)
		printf("\nKey:\t %u\n", this->gridCoords_key[dim]);
	for (int i = 0; i < ptNum; i++) {
		this->ptList[i]->showCoords();
	}
//	vector<GridCell*>& neighborList = this->getNeighboringCells();
	vector<GridCell*> neighborList;
	this->getNeighboringCells(neighborList);

	int neighborNum = neighborList.size();
	for (int i = 0; i < neighborNum; i++) {
		printf("Neighbor Coordinates: (");
		for (int j = 0; j < dim - 1; j++) {
			printf("%d, ", neighborList[i]->getCoords()[j]);
		}
		printf("%d)\n", neighborList[i]->getCoords()[dim - 1]);
	}

	printf("**********************************************\n");
}

unsigned int GridCell::computeModPrime(vector<int>& gridCoords) {
// Compute modPrime value
	unsigned long long temp = 0;
	unsigned int key = 0;
	int len = DIM;
	for (int i = 0; i < len; i++) {
		temp = (long long) gridCoords[i] * (long long) MY_RAND_INT[i];
		temp = (temp & MASK) + 5 * (temp >> 32);
		if (temp >= PRIME) {
			temp -= PRIME;
		}
		temp += key;
		if (temp >= PRIME) {
			temp -= PRIME;
		}
		key = (unsigned int) temp;
	}
	return key;
}

unsigned int GridCell::computeHashKey(vector<int>& gridCoords) {
	unsigned int key = computeModPrime(gridCoords);
//	temp = Para_A * key + Para_B;
//	key = (unsigned int) (temp % PRIME) % N;

//	key = key % N;
	return key;
}

bool GridCell::checkCellExistence(vector<int>& coords_key) {
	return this->gridTable->find(coords_key) != this->gridTable->end();
}

GridCell* GridCell::addCell(vector<int>& coords_key, bool computeNeighbor) {
	// create a new cell
	GridCell* cell = new GridCell(coords_key, this->gridTable->size() + 1,
			this->level + 1);
	GridCell::objectCnt++;

	// add cell to the hash table
	(*(this->gridTable))[cell->gridCoords_key] = cell;

	// If cell is a level-1 grid cell, then we need to compute its neighboring cells.
	// Otherwise, we skip this computation.
	if (cell->level == 1 && computeNeighbor) {

		if (USE_SPACE_FILLING) {
			this->bst.insert(cell);
		}
	}

	return cell;
}

GridCell* GridCell::getCell(vector<int>& coords_key) {
	if (checkCellExistence(coords_key))
		return (*(this->gridTable))[coords_key];
	else {
		return NULL;
	}
}

void GridCell::insertPointToCell(Point* pt) {
	// The point pt also records which cell it lies.
//	pt->gridCellList.push_back((void*) this);
	this->ptList.push_back(pt);
	this->size++;
	if (this->level > 1 && pt->isCore) {
		this->coreNum++;
	}
}

void GridCell::insertPointToTable(Point* pt, bool computeNeighbor) {
	vector<int> coords_key;
	coords_key.reserve(DIM + 1);

	// Use half of the current side length as the side length of the sub-grid cells.
//	pt->getGridCoords(coords_key, (this->oneOverSideLen * 2));
	pt->getGridCoords(coords_key, 2.0 / this->sideLen);
	coords_key.push_back(this->computeHashKey(coords_key));
	GridCell* target = this->getCell(coords_key);
	if (target == NULL)
		target = this->addCell(coords_key, computeNeighbor);
	target->insertPointToCell(pt);

	coords_key.clear();
}

void GridCell::constructGridCells(vector<Point*>& ptList) {

	if (this->gridTable == NULL) {
		this->gridTable = new hash_map<const vector<int>&, GridCell*, hash_func,
				key_equal>();
	}

	int ptNum = ptList.size();

	for (int i = 0; i < ptNum; i++) {
		this->insertPointToTable(ptList[i]);
	}

	printf("There are total %d non-empty cells.\n",
			(int) this->gridTable->size());
	fflush(stdout);
	/*
	 *  Output for debug.
	 */
//	int maxNum = -1;
//	HashTable::iterator it = this->gridTable->begin();
//	while (it != this->gridTable->end()) {
////		it->second->showInfo();
//		if (it->second->getSize() > maxNum) {
//			maxNum = it->second->getSize();
//		}
//		it++;
//	}
//	printf("The maximum number of points in a cell is %d.\n", maxNum);
//	fflush(stdout);
}

void GridCell::enumerateOffsets(GridCell* g, int dLev, vector<int>& cur,
		vector<GridCell*>& targetPlace) {
	unsigned int key = 0;
//	unsigned int temp = 0;
	int dim = DIM;
	GridCell* gTemp = NULL;
	if (dLev == dim - 1) {
		vector<int> targetCoords;
		targetCoords.reserve(dim + 1);

		// the grid cell g add the current offset
		for (int i = 0; i < dim; i++) {
			targetCoords.push_back(g->gridCoords_key[i] + cur[i]);
		}
		key = g->computeHashKey(targetCoords);
		targetCoords.push_back(key);
		gTemp = this->getCell(targetCoords);
		if (gTemp != NULL) {
			// this neighbor cell is non-empty
			targetPlace.push_back(gTemp);
		}

		// deal with the negative case
		if (cur[dLev] != 0) {
			targetCoords[dLev] = g->gridCoords_key[dLev] - cur[dLev];
			key = g->computeHashKey(targetCoords);
			targetCoords[dim] = key;
			gTemp = this->getCell(targetCoords);

			if (gTemp != NULL) {
				targetPlace.push_back(gTemp);
			}
		}
		return;
	}

	// recurse in the next dimension level
	// use the positive value of this dimension for recursion
	enumerateOffsets(g, dLev + 1, cur, targetPlace);
	if (cur[dLev] != 0) {

		// turn the value of this dimension to negative
		cur[dLev] = -cur[dLev];
		// use the negative value of this dimension for recursion
		enumerateOffsets(g, dLev + 1, cur, targetPlace);

		// turn the sign back
		cur[dLev] = -cur[dLev];
	}
}

vector<GridCell*>& GridCell::getNeighboringCells(GridCell* g,
		vector<GridCell*>& targetPlace) {

	// Clear the targetPlace.
	targetPlace.clear();

	int len = OFFSET_LIST.size();
	for (int i = 0; i < len; i++) {
		this->enumerateOffsets(g, 0, *(OFFSET_LIST[i]), targetPlace);
	}
	return targetPlace;
}

vector<GridCell*>& GridCell::getNeighboringCells(
		vector<GridCell*>& targetPlace) {
	if (GridCell::root != NULL) {
		if (USE_SPACE_FILLING) {
			GridCell::root->getNeighboringCells_BST2(this, targetPlace);
		} else {
			//Use the offset list.
			GridCell::root->getNeighboringCells(this, targetPlace);
		}
	}
	return targetPlace;
}

void GridCell::markCore(int index) {
	Point* pt = this->ptList[index];
	pt->isCore = true;
	if (this->coreNum != index) {
		this->ptList[index] = this->ptList[this->coreNum];
		this->ptList[this->coreNum] = pt;
	}
	this->coreNum++;
}

void GridCell::addAdjacentCell(GridCell* g) {
	this->adjacentList->insert(g);
}

/******************************************************************************
 *  The following functions are for grid tree of Approximate DBSCAN
 *****************************************************************************/

void GridCell::split(int lowestLevel) {

	if (this->level == lowestLevel
			|| (int) this->ptList.size() < CONSTANT_THRESHOLD) {
		this->isLeaf = true;
		return;
	}

	if (this->gridTable == NULL) {
		this->gridTable = new hash_map<const vector<int>&, GridCell*, hash_func,
				key_equal>;
	}

	// Only use the core point to build the tree structure.
	int coreNum = this->getCoreNum();
	for (int i = 0; i < coreNum; i++) {
		this->insertPointToTable(this->ptList[i]);
	}
	HashTable::iterator it;
	for (it = this->gridTable->begin(); it != this->gridTable->end(); it++) {
		it->second->split(lowestLevel);
	}
}

int GridCell::stateWithSphere(Point* q) {
	int dim = DIM;
	double closestDist = 0;
	double farthestDist = 0;
	double temp = 0;
	double temp2 = 0;
	double sqr_temp = 0;
	double sqr_temp2 = 0;
	double sqr_r = R * R;

	const double sideLen = this->sideLen;

// Find the distances from the closest and farthest points to q in this grid cell.
	for (int i = 0; i < dim; i++) {
		temp = this->gridCoords_key[i] * sideLen - q->coords[i];
		temp2 = temp + sideLen;
		sqr_temp = temp * temp;
		sqr_temp2 = temp2 * temp2;
		if (temp >= 0) {
			// q is to the left of this grid cell in this dimension
			closestDist += sqr_temp;
		} else if (temp2 <= 0) {
			// q is to the right of this grid cell in this dimension
			closestDist += sqr_temp2;
		}
		farthestDist += (sqr_temp <= sqr_temp2 ? sqr_temp2 : sqr_temp);
	}

	if (closestDist <= sqr_r) {
		if (farthestDist <= sqr_r)
			return 1; // fully inside
		return 0; // intersect
	}
	return -1; // fully outside
}

/*
 *******************************************************************
 *  The following are for getting neighboring cells by Z-order.
 *******************************************************************
 */

int GridCell::getMostSignificantBitPos(const vector<int>& v1,
		const vector<int>& v2) const {
	int pos = 0;
	int curMax = 0;
	int temp = 0;
	int dim = DIM;

	for (int i = 0; i < dim; i++) {
		temp = v1[i] ^ v2[i];
		if (curMax < temp && curMax < (temp ^ curMax)) {
			// Record the dimension where the different most significant bit lies.
			pos = i;
			curMax = temp;
		}
	}
	return pos;

}

vector<GridCell*>& GridCell::getNeighboringCells_BST(GridCell* g,
		vector<GridCell*>& targetPlace) {

// Clear the target place.
	targetPlace.clear();

// Find the given grid cell in BST.
	set<GridCell*, GridCellLess>::iterator it = this->bst.find(g);
	set<GridCell*, GridCellLess>::reverse_iterator left(it); // already the left of it.
	set<GridCell*, GridCellLess>::iterator right = it; // current the same position as it.

	const int stopCondition = ceil(sqrt(DIM));

// The range of possible coordinates of neighboring cells.
	vector<int> minCoords;
	vector<int> maxCoords;
	int dim = DIM;
	minCoords.resize(dim, 0);
	maxCoords.resize(dim, 0);

	int temp = 0;
	for (int i = 0; i < dim; i++) {
		temp = g->gridCoords_key[i] - stopCondition;
		if (temp > 0) { // All the coordinates should be non-negative.
			minCoords[i] = temp;
		}
		maxCoords[i] = temp + (stopCondition << 1);
	}

	if (it != this->bst.end()) {

		/***********Output Codes******************/
//		printf("++++++++++++++++++++++++++++++++++++++++++++\n");
//		printf("Current cell:\n");
//		g->showCoords();
////		printf("Current left:\n");
////		if (left != this->bst.rend())
////			(*left)->showInfo();
//		printf("The minCoords:\t");
//		for (int i = 0; i < DIM; i++) {
//			printf("%d\t", minCoords[i]);
//		}
//		printf("\n");
//
//		printf("The maxCoords:\t");
//		for (int i = 0; i < DIM; i++) {
//			printf("%d\t", maxCoords[i]);
//		}
//		printf("\n");
//		printf("Start checking:\n");
//		printf("Start checking the left direction:\n");
		/***********Output Codes******************/

		targetPlace.push_back((*it));

		while (left != this->bst.rend()
				&& (!compZorderByCoord((*left)->gridCoords_key, minCoords))) {
			if (isNeighbor(g->gridCoords_key, (*left)->gridCoords_key, dim)) {
				// Add this grid cell into target place as a neighbor cell of g.
				targetPlace.push_back((*left));
			}
			/***********Output Codes******************/
//			(*left)->showCoords();
			/***********Output Codes******************/
			left++;
		}

		right++;
		while (right != this->bst.end()
				&& (!compZorderByCoord(maxCoords, (*right)->gridCoords_key))) {
			if (isNeighbor(g->gridCoords_key, (*right)->gridCoords_key, dim)) {
				// Add this grid cell into target place as a neighbor cell of g.
				targetPlace.push_back((*right));
			}
			/***********Output Codes******************/
//			(*right)->showCoords();
			/***********Output Codes******************/

			right++;
		}
	}
	return targetPlace;
}

// The range of possible coordinates of neighboring cells.
vector<int> rangeMin;
vector<int> rangeMax;

vector<GridCell*>& GridCell::getNeighboringCells_BST2(GridCell* g,
		vector<GridCell*>& targetPlace) {
	targetPlace.clear();

	const int stopCondition = ceil(sqrt(DIM));

	int dim = DIM;
	rangeMin.resize(dim, 0);
	rangeMax.resize(dim, 0);

	int temp = 0;
	for (int i = 0; i < dim; i++) {
		temp = g->gridCoords_key[i] - stopCondition;
		if (temp > 0) { // All the coordinates should be non-negative.
			rangeMin[i] = temp;
		} else
			rangeMin[i] = 0;
		rangeMax[i] = temp + (stopCondition << 1);
	}

// Get the root of the Rb-tree.
	TreeNode root =
			(TreeNode) (((TreeNode) (this->bst.end()._M_node))->_M_parent);

	rangeQueryByZorder(root, rangeMin, rangeMax, g, targetPlace);

	return targetPlace;
}

void GridCell::rangeQueryByZorder(TreeNode curNode, vector<int>& curMin,
		vector<int>& curMax, GridCell* g, vector<GridCell*>& targetPlace) {

	if (curNode == NULL) {
		// The range query is done.
		return;
	}

#ifdef WIN
	GridCell* curCell = *(curNode->_M_valptr());
#else
	GridCell* curCell = curNode->_M_value_field;
#endif


// Get the coordinates of the current tree node.
	vector<int>& curCoords = curCell->gridCoords_key;

	int dim = DIM;

	bool isInRange = true;

	if (compZorderByCoord(curCoords, curMin)) {

		// If the Z-order value is smaller than that of rangeMin, then recurse in the right sub-tree.
		rangeQueryByZorder((TreeNode) (curNode->_M_right), curMin, curMax, g,
				targetPlace);
	} else if (compZorderByCoord(curMax, curCoords)) {

		// If the Z-order value is greater than that of rangeMax, then recurse in the left sub-tree.
		rangeQueryByZorder((TreeNode) (curNode->_M_left), curMin, curMax, g,
				targetPlace);
	} else {

		for (int i = 0; i < dim; i++) {
			if (curCoords[i] < rangeMin[i] || curCoords[i] > rangeMax[i]) {
				isInRange = false;
				break;
			}
		}

		if (isInRange) {

			if (isNeighbor(g->gridCoords_key, curCoords, dim)) {
				// Add this cell as a neighboring cell of g.
				targetPlace.push_back(curCell);
			}

			// Recurse in the left tree.
			rangeQueryByZorder((TreeNode) (curNode->_M_left), curMin, curCoords,
					g, targetPlace);

			// Recurse in the right tree.
			rangeQueryByZorder((TreeNode) (curNode->_M_right), curCoords,
					curMax, g, targetPlace);
		} else {

			vector<int> litMax(rangeMax);
			// Compute the LITMAX.
			computeLitMax(curCoords, rangeMin, rangeMax, litMax);

			// Recurse in the left tree.
			rangeQueryByZorder((TreeNode) (curNode->_M_left), curMin, litMax, g,
					targetPlace);

			vector<int> bigMin(rangeMin);
			// Compute the BIGMIN.
			computeBigMin(curCoords, rangeMin, rangeMax, bigMin);

			// Recurse in the right tree.
			rangeQueryByZorder((TreeNode) (curNode->_M_right), bigMin, curMax,
					g, targetPlace);
		}
	}
}

void GridCell::computeBigMin(vector<int>& curCoords, vector<int>& rangeMin,
		vector<int>& rangeMax, vector<int>& bigMin) {
	int dim = DIM;
	unsigned int stateCode = 0;
	unsigned int minMask;
	unsigned int maxMask;
	unsigned int zeroMask = 0;
	const unsigned int allOnes = ~0;
	vector<int> curMin(rangeMin);
	vector<int> curMax(rangeMax);

	for (int bit = WORD - 1; bit >= 0; bit--) {
		if (bit == 0)
			maxMask = 0;
		else
			maxMask = (allOnes >> (WORD - bit)); 			// 0111111111...
		minMask = 1 << bit; 								// 1000000000...
		if (bit != WORD - 1)
			zeroMask = (allOnes >> (bit + 1)) << (bit + 1); // 11111110000000... Set the lowest (bit+1) bits to 0 and the other bits are 1.

		for (int i = 0; i < dim; i++) {
			stateCode = (curCoords[i] >> bit) & 1;
			stateCode = (stateCode << 1) | ((curMin[i] >> bit) & 1);
			stateCode = (stateCode << 1) | ((curMax[i] >> bit) & 1);

			switch (stateCode) {
			case 0:
				// Do nothing.
				break;
			case 1:
//				curMin[i] = (curMin[i] & zeroMask) | minMask;
				for (int j = 0; j < dim; j++) {
					bigMin[j] = curMin[j];
				}
				bigMin[i] = (bigMin[i] & zeroMask) | minMask;
				curMax[i] = (curMax[i] & zeroMask) | maxMask;
				break;
			case 2:
				// Not possible. Do nothing.
				showError(
						"Error in GridCell::computeBigMin: the stateCode = 2 is incorrect!\n");
				break;
			case 3:
				// BIGMIN = current MIN.
				bigMin.swap(curMin);
				return;
			case 4:
				// BIGMIN is saved before.
//				// Minimize BIGMIN.
//				for (int j = 0; j < dim; j++) {
//					bigMin[j] = (bigMin[j] & ((allOnes >> bit) << bit));
//				}
				return;
			case 5:
				curMin[i] = (curMin[i] & zeroMask) | minMask;
				break;
			case 6:
				// Not possible. Do nothing.
				showError(
						"Error in GridCell::computeBigMin: the stateCode = 6 is incorrect!\n");
				break;
			case 7:
				// Do nothing.
				break;
			default:
				showError(
						"Error in GridCell::computeBigMin: the stateCode = -1 is incorrect!\n");
				break;
			}
		}
	}

}

void GridCell::computeLitMax(vector<int>& curCoords, vector<int>& rangeMin,
		vector<int>& rangeMax, vector<int>& litMax) {
	int dim = DIM;
	unsigned int stateCode = 0;
	unsigned int minMask;
	unsigned int maxMask;
	unsigned int zeroMask = 0;
	const unsigned int allOnes = ~0;
	vector<int> curMin(rangeMin);
	vector<int> curMax(rangeMax);

// The code is similar to that of BIGMIN but there are some differences.

	for (int bit = WORD - 1; bit >= 0; bit--) {
		if (bit == 0)
			maxMask = 0;
		else
			maxMask = (allOnes >> (WORD - bit)); 			// 0111111111...
		minMask = 1 << bit; 								// 1000000000...
		if (bit != WORD - 1)
			zeroMask = (allOnes >> (bit + 1)) << (bit + 1); // 11111110000000... Set the lowest (bit+1) bits to 0 and the other bits are 1.

		for (int i = 0; i < dim; i++) {
			stateCode = (curCoords[i] >> bit) & 1;
			stateCode = (stateCode << 1) | ((curMin[i] >> bit) & 1);
			stateCode = (stateCode << 1) | ((curMax[i] >> bit) & 1);

			switch (stateCode) {
			case 0:
				// Do nothing.
				break;
			case 1:
				curMax[i] = (curMax[i] & zeroMask) | maxMask;
				break;
			case 2:
				// Not possible. Do nothing.
				showError(
						"Error in GridCell::computeLitMax: the stateCode = 2 is incorrect!\n");
				break;
			case 3:
				// LITMAX is saved before.
//				// Maximize LITMAX.
//				for (int j = 0; j < dim; j++) {
//					litMax[j] = (litMax[j] & ((allOnes >> bit) << bit))
//							| maxMask;
//				}
				return;
			case 4:
				// LITMAX = curMax.
				litMax.swap(curMax);
				return;
			case 5:
//				curMax[i] = (curMax[i] & zeroMask) | maxMask;
				for (int j = 0; j < dim; j++) {
					litMax[j] = curMax[j];
				}
				litMax[i] = (litMax[i] & zeroMask) | maxMask;
				curMin[i] = (curMin[i] & zeroMask) | minMask;
				break;
			case 6:
				// Not possible. Do nothing.
				showError(
						"Error in GridCell::computeLitMax: the stateCode = 6 is incorrect!\n");
				break;
			case 7:
				// Do nothing.
				break;
			default:
				showError(
						"Error in GridCell::computeLitMax: the stateCode = -1 is incorrect!\n");
				break;
			}
		}
	}
}

