/*
 * GridCell.h
 *
 *  Created on: Sep 1, 2014
 *      Author: jhgan
 */

#ifndef GRIDCELL_H_
#define GRIDCELL_H_

#include "Point.h"
#include "Hashing.h"
#include "Offset.h"
#include "Zorder.h"
#include <ext/hash_map>
#include <set>
using namespace __gnu_cxx;

/*
 *  The class of grid cell.
 */
class GridCell {
protected:
	/*
	 *  The root of all grid cells. In fact, the root is a level-0 cell.
	 */
	static GridCell* root;

	/*
	 *  The ID of this grid cell.
	 */
	int ID;

	/*
	 *  The total number of points inside this grid cell.
	 */
	int size;

	/*
	 *  The total number of core points inside this grid cell.
	 */
	int coreNum;

	/*
	 *  The level of this grid cell.
	 */
	int level;

	/*
	 *  The side length of this grid cell
	 */
	double sideLen;

	/*
	 * 	The list of points inside this grid cell, where the first *coreNum* points are core points.
	 *
	 */
	vector<Point*> ptList;

	/*
	 * 	The coordinates of this grid cell and the hash key value at the last place of this vector.
	 */
	vector<int> gridCoords_key;

	/*
	 *******************************************************************
	 *  The following are some protected functions.
	 *******************************************************************
	 */

	/*
	 *  Compute the value of (coordinates * myRandInt) % PRIME.
	 *  Parameter List:
	 *  	gridCoords:		the given coordinate of a grid
	 */
	unsigned int computeModPrime(vector<int>& gridCoords);

	/*
	 *  Generate all possible offset from the offsetList.
	 *  Parameter List:
	 *  	g:				the pointer of the given grid cell
	 *  	dLev:			the current level of recursions
	 *  	cur:			the current coordinate of offsets at this level
	 *  	targetPlace:	the target place to store all the non-empty neighboring cells
	 */
	void enumerateOffsets(GridCell* g, int dLev, vector<int>& cur, vector<GridCell*>& targetPlace);

	/*
	 *  Get all the non-empty neighboring cells of a given level-1 grid cell g by detecting cells.
	 *  Parameter List:
	 *  	g:				the pointer of a given level-1 grid cell
	 *  	targetPlace:	the target place to store the neighboring cell of g
	 *  Return:
	 *  	the return the target place of non-empty neighboring cells of g
	 */
	vector<GridCell*>& getNeighboringCells(GridCell* g, vector<GridCell*>& targetPlace);

	/*
	 *  Get all the non-empty neighboring cells of a given level-1 grid cell g by the BST of Z-order.
	 *  Parameter List:
	 *  	g:				the pointer of a given level-1 grid cell
	 *  	targetPlace:	the target place to store the neighboring cell of g
	 *  Return:
	 *  	the return the target place of non-empty neighboring cells of g
	 */
	vector<GridCell*>& getNeighboringCells_BST(GridCell* g, vector<GridCell*>& targetPlace);

	vector<GridCell*>& getNeighboringCells_BST2(GridCell* g, vector<GridCell*>& targetPlace);

	/*******************************************************************/

	/*
	 *******************************************************************
	 *  The following are for getting neighboring cells by Z-order.
	 *******************************************************************
	 */

	/*
	 *  Get the index number of dimension where the different most significant bit lies.
	 *  Parameter List:
	 *  	v1: 		the first given coordinates
	 *  	v2:			the second given coordinates
	 *  Return:
	 *  	the index of the desired dimension
	 */
	int getMostSignificantBitPos(const vector<int>& v1, const vector<int>& v2) const;

	typedef std::_Rb_tree_iterator<GridCell*>::_Link_type TreeNode;

	/*
	 *  Process a range query by using the BST of Z-orders.
	 *  Parameter List:
	 *		curNode:			the current node of the Rb-tree
	 *		rangeMin:			the coordinates of the minimum point of the query range
	 *		rangeMax:			the coordinates of the maximum point of the query range
	 *		g:					the query grid cell
	 *		targetPlace:		the target place to store the neighboring cells found
	 */
	void rangeQueryByZorder(TreeNode curNode, vector<int>& curMin, vector<int>& curMax, GridCell* g,
			vector<GridCell*>& targetPlace);

	/*
	 *  Compute the BIGMIN value for the range query.
	 *  Parameter List:
	 *  	curCoords:			the coordinates of the dividing record point
	 *  	rangeMin:			the coordinates of the minimum point of the current query range
	 *  	rangeMax:			the coordinates of the maximum point of the current query range
	 *  	bigMin:				the target place to store the BIGMIN value
	 */
	void computeBigMin(vector<int>& curCoords, vector<int>& rangeMin, vector<int>& rangeMax,
			vector<int>& bigMin);

	/*
	 *  Compute the LITMAX value for the range query.
	 *  Parameter List:
	 *  	curCoords:			the coordinates of the dividing record point
	 *  	rangeMin:			the coordinates of the minimum point of the current query range
	 *  	rangeMax:			the coordinates of the maximum point of the current query range
	 *  	litMax:				the target place to store the BIGMIN value
	 */
	void computeLitMax(vector<int>& curCoords, vector<int>& rangeMin, vector<int>& rangeMax,
			vector<int>& litMax);

	/*******************************************************************/
public:

	static int objectCnt;

	/*
	 *  The comparison structure for the pointers of grid cells.
	 *  If the Z-order of g1 < that of g2, return TRUE. Otherwise, return FALSE.
	 */
	struct GridCellLess {
		bool operator()(const GridCell* g1, const GridCell* g2) const {
			int pos = 0;
			int curMax = 0;
			int temp = 0;
			int dim = DIM;
//			const vector<int>& v1 = g1->gridCoords_key;
//			const vector<int>& v2 = g2->gridCoords_key;

			for (int i = 0; i < dim; i++) {
				temp = g1->gridCoords_key[i] ^ g2->gridCoords_key[i];
				if (curMax < temp && curMax < (temp ^ curMax)) {
					// Record the dimension where the different most significant bit lies.
					pos = i;
					curMax = temp;
				}
			}
			return g1->gridCoords_key[pos] < g2->gridCoords_key[pos];
		}
	};

	/*
	 *  The Binary Search Tree for grid cells.
	 */
	static set<GridCell*, GridCellLess> bst;

	/*
	 *  The hash table storing all the sub-grid cells,
	 *  where the key is with type of vector<int>&.
	 */
	hash_map<const vector<int>&, GridCell*, hash_func, key_equal>* gridTable;

	/*
	 *  The constructor of GridCell.
	 *  Parameter List:
	 *  	gridCoords:		the coordinates of this grid cell
	 *  	ID:				the ID of this grid cell
	 *  	lev:			the level of this grid cell, in default lev = 1
	 */
	GridCell(vector<int>& gridCoords, int ID, int lev = 1);

	/*
	 *  The constructor of GridCell with specified side length.
	 *  Parameter List:
	 *  	sideLen:		the given side length
	 *  	gridCoords:		the coordinates of this grid cell
	 *  	ID:				the ID of this grid cell
	 *  	lev:			the level of this grid cell, in default lev = 1
	 */
//	GridCell(double sideLen, vector<int>& gridCoords, int ID, int lev = 1);

	/*
	 *  The constructor of level-0 grid cell.
	 */
	GridCell();

	~GridCell();

	/*
	 *  Set the root.
	 */
	static void setRoot(GridCell*);

	/*
	 *  Release the space of this grid cell.
	 */
	void releaseSpace();

	/*
	 *  Return the ID of this grid cell.
	 */
	int getID();

	/*
	 * 	Return the size of this grid cell.
	 */
	int getSize();

	/*
	 *  Return the level of this grid cell.
	 */
	int getLevel();

	/*
	 *  Return the number of core points in this grid cell.
	 */
	int getCoreNum();

	/*
	 *  Return the side length of this grid cell.
	 */
	double getSideLength();

	/*
	 *  Return the list of points in this grid cell.
	 */
	vector<Point*>& getPtList();

	/*
	 *  Return the coordinates of this grid cell.
	 */
	vector<int>& getCoords();

	/*
	 *  Set the side length of this grid cell.
	 *  Parameter List:
	 *  	sideLen:	the given side length which cannot be 0
	 */
	void setSideLength(double sideLen);

	/*
	 *  Show the info of this grid cell.
	 */
	void showInfo();

	/*
	 *  Show the coordinates of this grid cell.
	 */
	void showCoords();

	/*
	 *  Compute the hash key of the given grid coordinates.
	 *  Parameter List:
	 *  	girdCoords:		the given coordinates of a grid
	 *  Return:
	 *  	the resulting hash key
	 */
	unsigned int computeHashKey(vector<int>& gridCoords);

	/*
	 *  Insert a point to this grid cell.
	 *  Parameter List:
	 *  	pt:		the pointer of the point.
	 */
	void insertPointToCell(Point* pt);

	/*
	 *  Insert a point to the sub-grid hash table of this grid cell.
	 *  Parameter List:
	 *  	pt:					the pointer of the point.
	 *  	computeNeighbor:	If adding this point induces a new level-1 grid cell, this flag means whether to compute the neighbor cells
	 *  						for this newly added grid cell. In default, it is set to TRUE.
	 */
	void insertPointToTable(Point* pt, bool computeNeighbor = true);

	/*
	 *  Construct level-1 grid cells.
	 *  Parameter List:
	 *  	ptList:		the list of the pointers of points
	 */
	void constructGridCells(vector<Point*>& ptList);

	/*
	 * 	Check whether there exists the target grid cell.
	 * 	Parameter List:
	 * 		coords_key:	the coordinates and the hash key of the target grid cell
	 * 	Return:
	 * 		If the grid cell exists, return TRUE. Otherwise, return FALSE.
	 */
	bool checkCellExistence(vector<int>& coords_key);

	/*
	 *  Add a new cell into the hash table.
	 *  Parameter List:
	 * 		coords_key:			the coordinates and the hash key of the target grid cell
	 * 		computeNeighbor:	The flag means whether to compute the neighboring cells for this newly level-1 grid cell.
	 * 							In default, it is set to TRUE.
	 * 	Return:
	 * 		the pointer of the new grid cell
	 */
	GridCell* addCell(vector<int>& coords_key, bool computeNeighbor = true);

	/*
	 *  Get a grid cell by a key.
	 *  Parameter List:
	 *  	coords_key:	the coordinates and the hash key of the target grid cell
	 */
	GridCell* getCell(vector<int>& coords_key);

	/*
	 *  Get all the non-empty neighboring cells of a given level-1 grid cell g.
	 */
//	vector<GridCell*>& getNeighboringCells();
	vector<GridCell*>& getNeighboringCells(vector<GridCell*>& targetPlace);

	/*
	 *  Mark the point at *index* of *ptList* as a core point and then swap it
	 *  and the point ptList[coreNum]. Finally, coreNum++.
	 *  Parameter List:
	 *  	index:		the index of the target point at ptList.
	 */
	void markCore(int index);

	/*
	 ******************************************************************
	 *  The following functions are for grid tree.
	 ******************************************************************
	 */

	/*
	 *  Split this grid cell to at most 2^d sub-grid cells with
	 *  half of the current side length.
	 */
	void split(int lowestLevel);

	/*
	 *  Return the intersection state of this grid cell with
	 *  the sphere centered at q with radius r.
	 *  Parameter List:
	 *  	q:		the center point of the query sphere
	 *  Return:
	 *  	1:		this grid cell is fully inside the query sphere
	 *  	0:		this grid cell is intersected with the boundary of the query sphere
	 *  	-1:		this grid cell is fully outside the query sphere
	 */
	int stateWithSphere(Point* q);

	/*
	 ******************************************************************
	 *  The following members are for grid graph.
	 ******************************************************************
	 */

	/*
	 *  The flag of whether this vertex is processed. If yes, then it is TRUE. Otherwise, it is FALSE.
	 */
	bool isProcessed;

	/*
	 *  The flag for marking whether this cell is a leaf.
	 */
	bool isLeaf;

	/*
	 *  Compare two given pointers of GridCell by their IDs.
	 */
	struct GridCellIDComp {
		bool operator()(const GridCell* g1, const GridCell* g2) const {
			return g1->ID < g2->ID;
		}
	};

	/*
	 *  The list of the pointers of vertices such that there exist edges between this vertex and them.
	 */
	set<GridCell*, GridCellIDComp>* adjacentList;

	/*
	 *  Initialize the adjacent list for each grid cell.
	 */
	void initializeAdjacentLists();

	/*
	 *  Add an adjacent grid cell to this grid cell's adjacent list.
	 */
	void addAdjacentCell(GridCell* g);

};

typedef hash_map<const vector<int>&, GridCell*, hash_func, key_equal> HashTable;

#endif /* GRIDCELL_H_ */
