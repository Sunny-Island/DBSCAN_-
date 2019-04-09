/*
 * GlobalVar.cpp
 *
 *  Created on: Oct 7, 2014
 *      Author: jhgan
 */

#include "GlobalVar.h"

#ifdef WIN
#include <chrono>
#else
#include <sys/time.h>
#endif

/*****************************************************************************
 *  The following functions are for error processing.
 *****************************************************************************/

void showError(string msg) {
	printf("%s\n", msg.c_str());
	exit(0);
}

/*****************************************************************************
 *  The following functions are for running time evaluation.
 *****************************************************************************/

///*
// *  The data structure for getting time.
// */
//static struct timeval tim;
//double getCurrentTime() {
//	gettimeofday(&tim, NULL);
//	return tim.tv_sec + (tim.tv_usec / (double) CLOCKS_PER_SEC);
//}

#ifdef WIN
double getCurrentTime() {
	long long time = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count();
	return time / 1000000.0;
}
#else
/*
 *  The data structure for getting time.
 */
static struct timeval tim;
double getCurrentTime() {
	gettimeofday(&tim, NULL);
	return tim.tv_sec + (tim.tv_usec / (double) CLOCKS_PER_SEC);
}
#endif

/*****************************************************************************
 *  The global variables.
 *****************************************************************************/

/*
 *  The number of points.
 */
int N = -1;

/*
 *  The dimensionality of points.
 */
int DIM = -1;

/*
 *  The radius parameter for DBSCAN.
 */
double R = -1;

/*
 *  The core point threshold.
 */
int K = -1;

/*
 *  The approximation parameter for approximate DBSCAN.
 */
double PHI = -1;

/*
 *  The flag that whether to use the O(nk) algorithm for core point identification.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
bool USE_NK = true;

/*
 *  The flag that whether to use the space filling curve to get neighboring cells.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
bool USE_SPACE_FILLING = false;

/*
 *  The flag that whether to use the progressive method to construct grid graph.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
bool USE_PROGRESSIVE = false;

/*
 *  The flag that whether to use the space priority method to construct grid graph.
 *  If YES, this variable is TRUE. Otherwise, it is FALSE.
 */
bool USE_SPACE_PRIOR = false;

/*
 *  If a value is smaller than this threshold, then we regard this value is a constant.
 */
int CONSTANT_THRESHOLD = 100;

/*
 *  All possible offsets for getting neighboring cells with non-negative coordinates.
 */
vector<vector<int>*> OFFSET_LIST;

/*
 *  The lowest level of the data structure which is used in approximate DBSCAN.
 */
int LOWEST_LEVEL = 1;

double TimeRtreeRangeQuery = 0;
double TimeCheckMbr = 0;
long long TotalRangeQueryNum = 0;
long long TotalOpenedMbrNum = 0;
long long TotalDistComputation = 0;
long long TotalStateCheck = 0;

/*
 *  Compute the value of lowest level.
 */
void computeLowestLevel() {
	if (PHI > 0) {
		LOWEST_LEVEL = ceil(1 - log2(PHI));
		if (LOWEST_LEVEL < 1) {
			LOWEST_LEVEL = 1;
		}
	} else {
		LOWEST_LEVEL = 1;
	}
}
