/*
 * DataGenerator.h
 *
 *  Created on: Sep 24, 2014
 *      Author: jhgan
 */

#ifndef DATAGENERATOR_H_
#define DATAGENERATOR_H_

#include "headers.h"
#include <random>
#include <fstream>
using namespace std;

/*
 *********************************************************************************************
 *  This part is for generating data for test.
 *********************************************************************************************
 */

/*
 *  Generate a random real distributed uniformly in [rangeStart, rangeEnd]. Input must satisfy: rangeStart <= rangeEnd.
 *  The granularity of generated random reals is given by RAND_MAX.
 *  Parameter List:
 *  	rangeStart:		the start value of the range
 *  	rangeEnd:		the end value of the range
 *  Return:
 *  	a random real number in the given range.
 */
double genUniformRandom(double rangeStart, double rangeEnd);

/*
 *  Generate a random real from normal distribution N(0,1).
 */
double genGaussianRandom();

/*
 *  Generate a random point with integer coordinates expected within the given sphere.
 *  Parameter List:
 *  	center:				the center of the given sphere
 *  	radius:				the radius of the given sphere
 *  	dim:				the dimensionality of the data
 *  	targetPlace:		the target place to store the point generated
 */
void genPtFromGaussian(vector<int>& center, double radius, int dim, vector<int>& targetPlace);

/*
 *  Generate a random point with integer coordinates from Random.
 *  Parameter List:
 *  	dim:				the dimensionality of points
 *  	targetPlace:		the target place to store the point generated
 */
void genPtFromRandom(int dim, vector<int>& targetPlace, double rangeStart, double rangeEnd);

/*
 *  Generate a data set and write it into file.
 *  Parameter List:
 *  	filePath:			the path of target file
 *  	dim:				the dimensionality of data
 *  	totalNum:			the total number of point generated
 *  	clNum:				the number of centers
 *  	noiseNum:			the number of noise among totalNum of points
 *  	rangeStart:			the start value of the range of the coordinates
 *  	rangeEnd:			the end value of the range of the coordinates
 *  	radius:				the radius of the clusters generated
 */
void genDatasetToFile(char* filePath, int dim, int totalNum, int clNum, int noiseNum,
		double rangeStart, double rangeEnd, double radius, bool move = false);

/*
 *  Generate a random point within the given box.
 *  Parameter List:
 *  	dim:				the dimensionality of data
 *  	center:				the center of the given box
 *  	halfSideLength:		the half of the side length of the given box
 *  	targetPlace:		the target place to store the point generated
 */
void genPtInBox(int dim, vector<int>& center, double halfSideLength, vector<int>& targetPlace);

/*
 *  Generate a random point within the given $dim$-dimensional ball.
 *  Parameter List:
 *  	dim:				the dimensionality of data
 *  	center:				the center of the given box
 *  	radius:				the radius of the ball
 *  	targetPlace:		the target place to store the point generated
 */
void genPtInBall(int dim, vector<int>& center, double radius, vector<int>& targetPlace);

/*
 *  Generate a data set and write it into file.
 *  Parameter List:
 *  	filePath:			the path of target file
 *  	dim:				the dimensionality of data
 *  	totalNum:			the total number of point generated
 *  	expClNum:			the expected number of clusters
 *  	noiseNum:			the number of noise among the *totalNum* of points
 *  	rangeStart:			the minimum possible coordinate value
 *  	rangeEnd:			the maximum possible coordinate value
 *  	boxSideLenghth:		the side length of the box region
 *  	genPt:				the pointer of point generation function, like: genPtInBox, genPtInBall
 */
void genDatasetToFile(char* filePath, int dim, int totalNum, int expClNum, int noiseNum,
		double rangeStart, double rangeEnd, double boxSideLength,
		void (*genPt)(int, vector<int>&, double, vector<int>&));

/*
 *  A dataset generation function that automatically computes the other parameters.
 *  Parameter List:
 *  	filePath:			the path of target file
 *  	dim:				the dimensionality of data
 *  	totalNum:			the total number of point generated
 */
void genDatasetToFile(char* filePath, int dim, int totalNum);

/*
 *  Write a given point to file.
 *  Parameter List:
 *  	file:			the file handler
 *  	id:				the ID of the given point
 *  	coords:			the coordinates of the given point
 */
void writePointToFile(FILE* file, int id, vector<int>& coords);

/*
 *  Generate points in a given cell.
 *  Parameter List:
 *  	file:				the file handler
 *  	cell_coords:		the coordinates of the given cell
 *  	sideLen:			the side length of the given cell
 *  	ptNum:				the number of points inside the given cell
 */
void genPtsInCell(FILE* file, vector<int>& cell_coords, int sideLen, int ptNum, int startPtId);

#endif /* DATAGENERATOR_H_ */
