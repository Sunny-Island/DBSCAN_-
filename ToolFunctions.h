/*
 * ToolFunctions.h
 *
 *  Created on: Sep 23, 2014
 *      Author: jhgan
 */

#ifndef TOOLFUNCTIONS_H_
#define TOOLFUNCTIONS_H_

#include "headers.h"
#include "Point.h"
#include "GridCell.h"
#include "Cluster.h"

/*
 ******************************************************************
 *  The following functions are for Gunawan Exact DBSCAN
 ******************************************************************
 */

/*
 *  Check whether it can be added an edge between the two given grid cells.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeAddable_Exact(GridCell* p1, GridCell* p2, double r);

/*
 *  Check whether there is an edge between the two given grid cells by Nearest Neighbor query.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeAddable_Exact_NN(GridCell* g1, GridCell* g2, double r);

/*****************************************************************/

/*
 ******************************************************************
 *  The following functions are for our Exact DBSCAN
 ******************************************************************
 */

/*
 *  Compute the square of the minimum distance from a given point to a given cell.
 *  Parameter List:
 *  	p:		the given point
 *  	g:		the given cell
 *  Return:
 *  	The square of the minimum distance from p to g.
 */
double sqrDistFromPointToCell(Point* p, GridCell* g);

/*
 *  Compute the square of the minimum distance between the two given cells. Note that the two cells are
 *  not necessarily at the same level.
 */
double sqrDistBetweenCells(GridCell* g1, GridCell* g2);

/*
 *  Compute the square of the maximum distance between the two given cells. Note that the two cells are
 *  not necessarily at the same level.
 */
double sqrMaxDistBetweenCells(GridCell* g1, GridCell* g2);

/*
 *  Check whether it can be added an edge between the two given grid cells.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeAddable_BCP(GridCell* g1, GridCell* g2, double r);

/*
 *  Identify the core points by the O(nk) algorithm.
 *  Parameter List:
 *  	table:		the hash table containing all level-1 grid cells
 *  	r:			the radius for DBSCAN
 *  	k:			the core point threshold
 */
void identifyCorePts_nk(HashTable* table, double r, int k);

/*****************************************************************/

/*
 *  The list for recording which pair of grid cells has been processed.
 */
extern set<pair<GridCell*, GridCell*> > processedList;


/*
 *  The variable for recording the time for sorting.
 */
extern double SORTINGTIME;

extern double PreSORTINGTIME;

/*
 *  Clear the current processedList and curComponent.
 */
void clearLists();

/*
 *  Check whether there already is an edge or path between the two given cells.
 *  If YES, return TRUE. Otherwise, return FALSE.
 */
bool edgeExists_Progressive(GridCell* g1, GridCell* g2);

/*
 *  Assign the core points of the given grid cell to the cluster with ID = %clusterID.
 */
void assignCorePtsToCluster(GridCell* g, int clusterID);

/*
 *  Find the connected component of grid cells.
 */
int expandFromGridCell(GridCell* g, double r, bool (*edgeExists)(GridCell*, GridCell*),
		bool (*edgeAddable)(GridCell*, GridCell*, double));

int expandFromGridCell(GridCell* g, double r, bool (*edgeExists)(GridCell*, GridCell*),
		bool (*edgeAddable)(GridCell*, GridCell*, double), int clusterID);

#endif /* TOOLFUNCTIONS_H_ */
