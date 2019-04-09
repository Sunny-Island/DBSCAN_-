/*
 * Rtree.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "Rtree.h"

Rtree::Rtree() {
	this->root = NULL;
}

Rtree::~Rtree() {
	this->releaseSpace();
}

void Rtree::releaseSpace() {
	if (this->root != NULL) {
		// Release the space of R-tree.
		this->root->releaseSpace();
		delete (this->root);
		this->root = NULL;
		RtreeNode::objectCnt--;
	}
}

/*
 *  Compare function for Element.
 */
bool comp(const RtreeElement* e1, const RtreeElement* e2) {
	return compCode(e1->zdr, e2->zdr);
}

/*
 *  Compare the Z-order of the two given points.
 *  Return:
 *  	If the Z-order of p1 is <= that of p2, return TRUE. Otherwise, return FALSE.
 */
bool comp_Zorder(const Point* p1, const Point* p2) {
	return compZorderByCoord(p1->coords, p2->coords);
}

/*
 *  The comparison structure for the two given points in set.
 */
struct ltstr {
	bool operator()(const Point* p1, const Point* p2) const {
		return compZorderByCoord(p1->coords, p2->coords);
	}
};

void Rtree::bulkLoadRtree(vector<Point*>& ptList, int fanout, int bitNum) {

	int num = ptList.size();
	if (num == 0) {
		showError("Error in Rtree::bulkLoadRtree: the point list is empty!\n");
		return;
	} else if (num == 1) {
		this->root = new RtreeNode(ptList[0]);
		RtreeNode::objectCnt++;
		return;
	}

	/*************** Codes for debug ****************/
//	set<Point*, ltstr> bst;
//	for (int i = 0; i < num; i++) {
//		bst.insert(ptList[i]);
//	}
//	set<Point*, ltstr>::iterator it = bst.find(ptList[4]);
//	while (it != bst.end()) {
//		(*it)->showCoords();
//		it--;
//	}
//
//	printf("\n");
	/*************************************************/

	queue<RtreeNode*> que, que2;
	queue<RtreeNode*>& work = que;
	queue<RtreeNode*>& store = que2;

	/*************** Sort directly by the coordinates of points ****************/

	sort(ptList.begin(), ptList.end(), comp_Zorder);
	for (int i = 0; i < num; i++) {
		que.push(new RtreeNode(ptList[i]));
		RtreeNode::objectCnt++;
	}

//	/*************** Codes for debug ****************/
//	for (int i = 0; i < num; i++) {
//		ptList[i]->showCoords();
//	}
//	printf("\n");
	/*************************************************/

	RtreeNode* parent = NULL;
	bool flag = false;
	while (work.size() + store.size() != 1) {
		parent = new RtreeNode();
		RtreeNode::objectCnt++;

		for (int i = 0; i < fanout; i++) {
			if (!work.empty()) {
				parent->addChildNode(work.front());
				work.pop();
			} else {
				flag = true;
				break;
			}
		}

		if (flag) {
			if (parent->getChildNodes().size() > 0) {
				store.push(parent);
			} else {
				delete (parent);
				RtreeNode::objectCnt--;
			}
			std::swap(work, store);
			flag = false;
		} else {
			store.push(parent);
		}
	}
	if (work.empty())
		this->root = store.front();
	else
		this->root = work.front();
}

void Rtree::rangeQuery(Point* pt, double r, vector<Point*>& targetPlace) {
	if (this->root == NULL) {
		showError("Error in Rtree::rangeQuery: The root is NULL!\n");
	}

	TotalRangeQueryNum++;

//	double start1 = 0;
//	double end1 = 0;

	// Clear the targetPlace.
	targetPlace.clear();

	double sqr_r = r * r;
	int childNum = 0;
	int state = 0;
	double sqrDist = 0;

	queue<RtreeNode*> que;
	que.push(this->root);
	RtreeNode* cur_node = NULL;

	while (!que.empty()) {
		cur_node = que.front();
		que.pop();

		TotalOpenedMbrNum++;

		// If the current node is a leaf, compute the distance from query point to the point inside this leaf.
		if (cur_node->isLeaf()) {
			sqrDist = pt->getSqrDist(cur_node->getPoint());

			TotalDistComputation++;

			if (sqrDist <= sqr_r)
				targetPlace.push_back(cur_node->getPoint());
		} else {
			// If the current node is not a leaf, check its child nodes and add those intersecting with the rectangle of q to the queue.
			vector<RtreeNode*>& childList = cur_node->getChildNodes();
			childNum = childList.size();
			for (int i = 0; i < childNum; i++) {
//				start1 = getCurrentTime();
				state = childList[i]->stateWithSphere(pt, r);
//				end1 = getCurrentTime();
//				TimeCheckMbr += end1 - start1;

				TotalStateCheck++;

				if (state != -1) {
					// intersect or fully inside, then add this tree node to queue
					que.push(childList[i]);
				}
			}

		}
	}
}

void Rtree::showRtree() {
	RtreeNode* curNode = this->root;
	if (curNode == NULL) {
		showError("Error in Rtree::showRtree: The root is NULL!\n");
	}

	int childNum = 0;

	queue<RtreeNode*> que;
	que.push(curNode);
	while (!que.empty()) {
		curNode = que.front();
		que.pop();

		curNode->showTreeNode();

		vector<RtreeNode*>& childList = curNode->getChildNodes();
		childNum = childList.size();
		for (int i = 0; i < childNum; i++) {
			que.push(childList[i]);
		}
	}
}

bool Rtree::isValid() {
	return this->root != NULL;
}
