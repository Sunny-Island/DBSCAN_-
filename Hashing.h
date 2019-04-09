/*
 * Hashing.h
 *
 *  Created on: Sep 14, 2014
 *      Author: jhgan
 */

#ifndef HASHING_H_
#define HASHING_H_

#include "headers.h"

struct hash_func {
	size_t operator()(const vector<int>& hash_key) const {
//		unsigned long long temp = 0;
//		temp = Para_A * hash_key[hash_key.size() - 1] + Para_B;
//		temp = (temp % PRIME) % N;
		return (unsigned int) hash_key[hash_key.size() - 1];
	}
};

struct key_equal {
	bool operator()(const vector<int>& g1, const vector<int>& g2) const {
		int len = g1.size();
		if (g1[len - 1] != g2[len - 1])
			return false;
		for (int i = 0; i < len - 1; i++) {
			if (g1[i] != g2[i])
				return false;
		}
		return true;
	}
};

/*****************************************************************************
 *  The following variables are for hashing function.
 *****************************************************************************/

/*
 *  A big enough prime number 2^32 - 5.
 */
static const unsigned int PRIME = -5;

/*
 *  2^32 - 1
 */
static const unsigned int MASK = -1;

/*
 *  2^29
 */
static const unsigned int RANGE = (1 << 29);

/*
 *  Random integers from the range of [1, 2^29].
 */
extern vector<int> MY_RAND_INT;

/*
 *  Initialize the random variables.
 *  Parameter List:
 *  	dim:	the dimensionality of points
 */
void initializeRandom(int dim, vector<int>& myRandInt);


void initializeRandom();

/*****************************************************************************/

#endif /* HASHING_H_ */
