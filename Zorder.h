/*
 * Zorder.h
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#ifndef ZORDER_H_
#define ZORDER_H_

#include "headers.h"

/*******************************************************************
 *  The following functions are for Z-order.
 *******************************************************************/

typedef vector<unsigned int> Zorder;

/*
 *  The number of bits in a int value.
 */
static const int WORD = 32;

/*
 *  Encode a given coordinates into Z-order.
 *  Parameter List:
 *  	coords:		the given coordinates
 *  	zdr:		the target place to store the resulting Z-order
 *  	bitNum:		the number of bits of each coordinates (bitNum = 32 in default)
 */
void encode(int* coords, Zorder& zdr, int bitNum = WORD);

/*
 *  Compare the two given Z-orders.
 *  Return:
 *  	If z1 is "smaller" than z2, return TRUE. Otherwise, return FALSE.
 */
bool compCode(const Zorder& z1, const Zorder& z2);

/*
 *  Print out the Z-order string.
 *  Parameter List:
 *  	zdr:		the Z-order value
 *  	dim:		the dimensionality of points
 *  	bitNum:		the number of bits of each coordinates (bitNum = 32 in default)
 */
void showCode(Zorder& zdr, int dim, int bitNum = WORD);

/*******************************************************************/

/*
 *  Compare the Z-order directly by the original coordinates.
 *  Return:
 *  	If the Z-order of v1 is <= that of v2, return TRUE. Otherwise, return FALSE.
 */
bool compZorderByCoord(const int* v1, const int* v2);

bool compZorderByCoord(const vector<int>& v1, const vector<int>& v2);

#endif /* ZORDER_H_ */
