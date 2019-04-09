/*
 * RtreeNode.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "RtreeNode.h"

int RtreeNode::objectCnt = 0;

RtreeNode::RtreeNode() {
	// Means that this node currently is empty and invalid.
	this->level = -1;
	this->pt = NULL;
}

RtreeNode::RtreeNode(Point* pt) {
	this->level = 0;
	this->pt = pt;
	this->mbr.setMinCoords(pt->coords);
	this->mbr.setMaxCoords(pt->coords);
}

RtreeNode::~RtreeNode() {
	this->releaseSpace();
}

void RtreeNode::releaseSpace() {
	int num = this->childNodes.size();
	if (num == 0)
		return;
	for (int i = 0; i < num; i++) {
		this->childNodes[i]->releaseSpace();
		delete (this->childNodes[i]);
		this->childNodes[i] = NULL;
		RtreeNode::objectCnt--;
	}
	this->childNodes.clear();
}

bool RtreeNode::isLeaf() {
	return this->level == 0;
}

Point* RtreeNode::getPoint() {
	return this->pt;
}

vector<RtreeNode*>& RtreeNode::getChildNodes() {
	return this->childNodes;
}

void RtreeNode::addChildNode(RtreeNode* child) {
	// Update the MBR for the parent node.
	if (this->level == -1) {
		this->mbr.setByRectangle(child->mbr);
		this->level = child->level + 1;
	}

	this->mbr.enlarge(child->mbr);
	this->childNodes.push_back(child);
}

int RtreeNode::stateWithSphere(Point* q, double r) {
	return this->mbr.stateWithSphere(q, r);
}

void RtreeNode::showTreeNode() {
	printf("========================================================\n");
	printf("Level: %d\n", this->level);
	this->mbr.showRectangle();
}

