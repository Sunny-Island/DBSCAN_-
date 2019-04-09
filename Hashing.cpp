/*
 * Hashing.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "Hashing.h"

/*
 *  Random integers from the range of [1, 2^29].
 */
vector<int> MY_RAND_INT;

/*
 *  Initialize the random variables.
 *  Parameter List:
 *  	dim:	the dimensionality of points
 */
void initializeRandom(int dim, vector<int>& myRandInt) {
	myRandInt.clear();
	srand(time(NULL));
	for (int i = 0; i < dim; i++) {
		myRandInt.push_back(rand() % RANGE + 1);
	}
}

void initializeRandom() {
	MY_RAND_INT.clear();
	srand(time(NULL));
	for (int i = 0; i < DIM; i++) {
		MY_RAND_INT.push_back(rand() % RANGE + 1);
	}
}
