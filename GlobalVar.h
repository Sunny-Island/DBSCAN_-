/*
 * GlobalVar.h
 *
 *  Created on: Sep 2, 2014
 *      Author: jhgan
 */

#ifndef GLOBALVAR_H_
#define GLOBALVAR_H_

#include <string>
#include <vector>
#include <math.h>


using namespace std;

/*****************************************************************************
 *  The following functions are for error processing.
 *****************************************************************************/

void showError(string msg);

/*****************************************************************************
 *  The following functions and variables are for running time evaluation.
 *****************************************************************************/

double getCurrentTime();

/*****************************************************************************
 *  The global variables.
 *****************************************************************************/

/*
 *  The number of points.
 */
extern int N;

/*
 *  The dimensionality of points.
 */
extern int DIM;

/*
 *  The radius parameter for DBSCAN.
 */
extern double R;

/*
 *  The core point threshold.
 */
extern int K;

/*
 *  The approximation parameter for approximate DBSCAN.
 */
extern double PHI;

/*
 *  The flag that whether to use the O(nk) algorithm for core point identification.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
extern bool USE_NK;

/*
 *  The flag that whether to use the space filling curve to get neighboring cells.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
extern bool USE_SPACE_FILLING;

/*
 *  The flag that whether to use the progressive method to construct grid graph.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
extern bool USE_PROGRESSIVE;

/*
 *  The flag that whether to use the space priority method to construct grid graph.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
extern bool USE_SPACE_PRIOR;

/*
 *  If a value is smaller than this threshold, then we regard this value is a constant.
 */
extern int CONSTANT_THRESHOLD;

/*
 *  All possible offsets for getting neighboring cells with non-negative coordinates.
 */
extern vector<vector<int>*> OFFSET_LIST;

/*
 *  The lowest level of the data structure which is used in approximate DBSCAN.
 */
extern int LOWEST_LEVEL;

extern double TimeRtreeRangeQuery; /* The total time for Rtree range queries. */
extern double TimeCheckMbr;
extern long long TotalRangeQueryNum;
extern long long TotalOpenedMbrNum;
extern long long TotalDistComputation;
extern long long TotalStateCheck;

/*
 *  Compute the value of lowest level.
 */
void computeLowestLevel();

#endif /* GLOBALVAR_H_ */
