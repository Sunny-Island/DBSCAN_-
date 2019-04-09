/*
 * Rectangle.h
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include "headers.h"
#include "Point.h"

class Rectangle {
protected:

public:
	/*
	 *  The list of lower coordinates.
	 */
	int* minValues;

	/*
	 *  The list of higher coordinates.
	 */
	int* maxValues;

	Rectangle();

	/*
	 *  The constructor with parameters.
	 */
	Rectangle(int* minValues, int* maxValues);

	~Rectangle();

	/*
	 *  Detect whether this rectangle intersects with the given rectangle *rec*.
	 *  Return:
	 *  	If YES, return TRUE. Otherwise, return FALSE.
	 */
	bool intersect(const Rectangle& rec) const;

	/*
	 *  Detect whether this rectangle is fully inside the given rectangle *rec*.
	 *  Return:
	 *  	If YES, return TRUE. Otherwise, return FALSE.
	 */
	bool inside(const Rectangle& rec) const;

	/*
	 *  Find the minimum bounding rectangle of this and the given rectangles.
	 *  Parameter List:
	 *  	rec:			the given rectangle
	 *  Return:
	 *  	the resulting MBR
	 */
	Rectangle& enlarge(const Rectangle& rec);

	/*
	 *  Set this rectangle by a given rectangle.
	 */
	void setByRectangle(const Rectangle& rec);

	/*
	 *  Set the list of lower values.
	 */
	void setMinCoords(const int* minCoords);

	/*
	 *  Set the list of higher values.
	 */
	void setMaxCoords(const int* maxCoords);


	/*
	 *  Check the state of the MBR of this node with the sphere centered at q with radius r.
	 *  Parameter List:
	 *  	q:			the query point
	 *  	r:			the radius of the sphere
	 *  Return:
	 *  	1:		the MBR is fully inside the query sphere
	 *  	0:		the MBR is intersected with the boundary of the query sphere
	 *  	-1:		the MBR is fully outside the query sphere
	 */
	int stateWithSphere(Point* q, double r);

	/*
	 *  Show the coordinates of this rectangle.
	 */
	void showRectangle();

};

#endif /* RECTANGLE_H_ */
