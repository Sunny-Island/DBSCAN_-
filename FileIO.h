/*
 * FileIO.h
 *
 *  Created on: Sep 14, 2014
 *      Author: jhgan
 */

#ifndef FILEIO_H_
#define FILEIO_H_

#include "headers.h"
#include "Point.h"
#include <fstream>
using namespace std;

/*
 *********************************************************************
 *  The following functions are for the data set file I/O.
 *********************************************************************
 */

/*
 *  Read a data point from file.
 */
Point* readPointFromFile(FILE* fileHandle, int dim);

/*
 *  Read the dataset from file.
 *  Parameter List:
 *  	filePath:		the path of the given file
 *  	targetPlace:	the target place to store all the points read from file
 *  Return:
 *  	the pair<int, int> of minimum and maximum coordinate values
 */
std::pair<int, int> readDatasetFromFile(char* filePath, vector<Point*>& targetPlace);

/*
 *  Write a point to the specified file.
 *  Parameter List:
 *  	fileHandle:		the handle of the file
 *  	pt:				the point needs to be written to file
 */
void writePointToFile(FILE* fileHandle, Point* pt);

/*
 *  Write the given cluster to the specified file.
 *  Parameter List:
 *  	filePath:		the path of the specified file
 *  	cluster:		the given cluster
 */
void writeClusterToFile(char* filePath, vector<Point*>& cluster);

/*
 *  Write the given cluster to the specified file.
 *  Parameter List:
 *  	filePath:		the path of the specified file
 *  	cluster:		the given cluster in the form of a set of point IDs
 */
void writeClusterToFile(char* filePath, set<Point*>* cluster);

/********************************************************************/

/*
 *********************************************************************
 *  The following functions are for the ground truth file I/O.
 *********************************************************************
 */

/*
 *  Write the clustering result to file.
 *  Parameter List:
 *  	clusterList:	the list of resulting clusters
 *  	filePath:		the path of the target file
 */
void writeGroundTruth(vector<set<Point*>*>& clusterList, char* filePath);

/*
 *  Read the clustering ground truth from file.
 *  Parameter List:
 *  	clusterList:			the target place to store the ground truth
 *  	representCorePtList:	the representative core point for each cluster
 *  	filePath:				the path of the target file
 */
void readGroundTruth(vector<set<int> >& clusterList, vector<int>& representCorePtList,
		char* filePath);

/********************************************************************/

/*
 *********************************************************************
 *  The following functions are for scaling the datasets.
 *********************************************************************
 */

float* readCoordsFromFile(FILE* file, int dim);

void writeIntCoordsToFile(FILE* file, int* coords, int dim);

void normalizeDataset_Float(char* inputFilePath, char* outputFilePath, int rangeStart,
		int rangeEnd);

int* readIntCoordsFromFile(FILE* file, int dim);

void normalizeDataset_Int(char* inputFilePath, char* outputFilePath, int rangeStart, int rangeEnd);

/*
 *  Read the coordinates of the points from the household electric usage data set.
 */
float* readCoordsFromHousehold(FILE* file, int dim);

/*
 * Read the coordinates of the points from the SUSY data set.
 */
float* readCoordsFromSUSY(FILE* file, int dim);

/*
 *  The abstract read and write function.
 */
void readAndWriteDataset_Float(char* inputFilePath, char* outputFilePath, int rangeStart,
		int rangeEnd, float* (*readMethod)(FILE*, int));

/*
 *  Read and write the coordinates of the household data set.
 */
void readAndWriteHousehold(char* inputFilePath, char* outputFilePath, int rangeStart, int rangeEnd);

/*
 *  Read and write the coordinates of the SUSY data set.
 */
void readAndWriteSUSY(char* inputFilePath, char* outputFilePath, int rangeStart, int rangeEnd);

/*
 *  Drop the last <dropNum> dimensions from the coordinates.
 */
void dropDim(char* inputFilePath, char* outputFilePath, int dropNum);

/********************************************************************/

#endif /* FILEIO_H_ */
