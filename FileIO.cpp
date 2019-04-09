/*
 * FileIO.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "FileIO.h"

/*
 *********************************************************************
 *  The following functions are for the data set file I/O.
 *********************************************************************
 */

/*
 *  A buffer for reading some useless char.
 */
char sBuffer[100];

/*
 *  Read a data point from file.
 */
Point* readPointFromFile(FILE* fileHandle, int dim, std::pair<int, int>& min_max) {
	if (fileHandle == NULL) {
		showError("Error in function readPointFromFile:\nFile handle is NULL!\n");
	}
	int id = 0;
	int* coords = new int[dim];

	fscanf(fileHandle, "%d", &id);
	for (int i = 0; i < dim; i++) {
		fscanf(fileHandle, "%d", &(coords[i]));

		if (coords[i] > min_max.second) {
			min_max.second = coords[i];
		}

		if (coords[i] < min_max.first) {
			min_max.first = coords[i];
		}

	}
	// eat the useless char
	fscanf(fileHandle, "%[^\n]", sBuffer);
	Point* pt = new Point(id, coords);
	if (pt == NULL) {
		showError("Error in function readPointFromFile:\nCannot new a Point!\n");
	}
//	delete[] coords;
	return pt;
}

/*
 *  Read the dataset from file.
 *  Parameter List:
 *  	filePath:		the path of the given file
 *  	targetPlace:	the target place to store all the points read from file
 *  Return:
 *  	the pair<int, int> of minimum and maximum coordinate values
 */
std::pair<int, int> readDatasetFromFile(char* filePath, vector<Point*>& targetPlace) {
	FILE* file = fopen(filePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function readDatasetFromFile:\nFile open failed! Please check your file path!\n");
	}

	std::pair<int, int> min_max(2147483647, -2147483647);

	int n = 1, dim = 2;
//	fscanf(file, "%d %d", &n, &dim);
	fscanf(file, "%d", &n);
	fscanf(file, "%d", &dim);
	// eat the useless char
	fscanf(file, "%[^\n]", sBuffer);

	targetPlace.clear();
	targetPlace.reserve(n);
	for (int i = 0; i < n; i++) {
		targetPlace.push_back(readPointFromFile(file, dim, min_max));
		if (i >= 100000 && i % 100000 == 0) {
			printf("\n%d points read.", i);
			fflush(stdout);
		}
	}
	printf("\n%d points read.\n", n);
	fclose(file);
	return min_max;
}

void writePointToFile(FILE* fileHandle, Point* pt) {
	if (fileHandle == NULL) {
		showError("Error in function writePointToFile:\nFile handle is NULL!\n");
	}

	int dim = DIM;
	fprintf(fileHandle, "%d", pt->id);
	for (int i = 0; i < dim; i++) {
		fprintf(fileHandle, "\t%d", pt->coords[i]);
	}
	fprintf(fileHandle, "\n");
}

void writeClusterToFile(char* filePath, vector<Point*>& cluster) {
	FILE* file = fopen(filePath, "wt");
	if (file == NULL) {
		showError(
				"Error in function writeClusterToFile:\nFile open failed! Please check your file path!\n");
	}
	int ptNum = cluster.size();
	for (int i = 0; i < ptNum; i++) {
		writePointToFile(file, cluster[i]);
	}
	fclose(file);
}

void writeClusterToFile(char* filePath, set<Point*>* cluster) {
	FILE* file = fopen(filePath, "wt");
	if (file == NULL) {
		showError(
				"Error in function writeClusterToFile:\nFile open failed! Please check your file path!\n");
	}

	set<Point*>::iterator it = cluster->begin();

	fprintf(file, "%d\n", (int) cluster->size());

	while (it != cluster->end()) {
		writePointToFile(file, (*it));
		it++;
	}
	fclose(file);
}

/********************************************************************/

/*
 *********************************************************************
 *  The following functions are for the ground truth file I/O.
 *********************************************************************
 */

void writeGroundTruth(vector<set<Point*>*>& clusterList, char* filePath) {

	int clusterCnt = clusterList.size();

	vector<int> representCorePtList;
	representCorePtList.resize(clusterCnt, -1);

	set<Point*>::iterator it;

	for (int i = 0; i < clusterCnt; i++) {
		it = clusterList[i]->begin();
		while (it != clusterList[i]->end()) {
			if ((*it)->isCore) {
				representCorePtList[i] = (*it)->id;
				break;
			}
			it++;
		}
	}

	printf("Start writing the ground truth.\n");
	// Write the cluster list to file.
	FILE* file = fopen(filePath, "wt");
	if (file == NULL) {
		showError(
				"Error in function writeGroundTruth:\nFile open failed! Please check your file path!\n");
	}

	// Write the cluster number to file.
	fprintf(file, "%d\n", clusterCnt);

	int ptNum = 0;
	for (int i = 0; i < clusterCnt; i++) {
		fprintf(file, "%d\t", representCorePtList[i]);
		ptNum = clusterList[i]->size();
		fprintf(file, "%d\t", ptNum);

		it = clusterList[i]->begin();

		while (it != clusterList[i]->end()) {
			fprintf(file, "%d ", (*it)->id);
			it++;
		}
		fprintf(file, "\n");
	}
	printf("Finish writing the ground truth.\n");
	fflush(stdout);

	fclose(file);
}

void readGroundTruth(vector<set<int> >& clusterList, vector<int>& representCorePtList,
		char* filePath) {
	printf("Start reading the ground truth.\n");

	FILE* file = fopen(filePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function readGroundTruth:\nFile open failed! Please check your file path!\n");
	}

	int clusterCnt = 0;
	fscanf(file, "%d\n", &clusterCnt);
	clusterList.resize(clusterCnt);
	representCorePtList.resize(clusterCnt, -1);
	int clID = -1;
	int ptNum = 0;

	for (int i = 0; i < clusterCnt; i++) {
		fscanf(file, "%d\t", &(representCorePtList[i]));
		fscanf(file, "%d\t", &ptNum);
		for (int j = 0; j < ptNum; j++) {
			fscanf(file, "%d ", &clID);
			clusterList[i].insert(clID);
		}
		// eat the useless char
//		fscanf(file, "%c", sBuffer);
	}

	printf("Finish reading the ground truth.\n");

	fclose(file);
}

/********************************************************************/

/*
 *********************************************************************
 *  The following functions are for scaling the datasets.
 *********************************************************************
 */

float* readCoordsFromFile(FILE* file, int dim) {
	float* coords = new float[dim];
	for (int i = 0; i < dim; i++) {
		fscanf(file, "%f", &(coords[i]));
	}
	// eat the useless char
	fscanf(file, "%[^\n]", sBuffer);
	return coords;
}

void writeIntCoordsToFile(FILE* file, int id, int* coords, int dim) {
	fprintf(file, "%d", id);

	for (int i = 0; i < dim; i++) {
		fprintf(file, " %d", coords[i]);
	}
	fprintf(file, "\n");
}

void normalizeDataset_Float(char* inputFilePath, char* outputFilePath, int rangeStart,
		int rangeEnd) {
	FILE* file = fopen(inputFilePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function normalizeDataset:\nInput file open failed! Please check your input file path!\n");
	}

	vector<float*> ptList;
	ptList.reserve(N);

	vector<float> maxList;
	maxList.resize(DIM, -1000000000000000);
	vector<float> minList;
	minList.resize(DIM, 1000000000000000);
	float* coords = NULL;

	for (int i = 0; i < N; i++) {
		coords = readCoordsFromFile(file, DIM);
		ptList.push_back(coords);

		for (int j = 0; j < DIM; j++) {
			if (coords[j] > maxList[j]) {
				maxList[j] = coords[j];
			}
			if (coords[j] < minList[j]) {
				minList[j] = coords[j];
			}
		}
	}

	fclose(file);

	FILE* outputFile = fopen(outputFilePath, "wt");
	if (outputFile == NULL) {
		showError(
				"Error in function normalizeDataset:\n Output file open failed! Please check your output file path!\n");
	}

	fprintf(outputFile, "%d %d\n", N, DIM);

	vector<float> MMList;
	MMList.resize(DIM);
	for (int i = 0; i < DIM; i++) {
		MMList[i] = maxList[i] - minList[i];
	}
	int rangeLength = rangeEnd - rangeStart;
	int* intCoords = new int[DIM];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < DIM; j++) {
			if (MMList[j] != 0) {
				intCoords[j] = (int) ((((ptList[i][j] - minList[j]) / MMList[j]) * rangeLength))
						+ rangeStart;
			} else {
				intCoords[j] = rangeStart;
			}
		}

		writeIntCoordsToFile(outputFile, i + 1, intCoords, DIM);

		delete (ptList[i]);
		ptList[i] = NULL;
	}

	delete intCoords;
	ptList.clear();

	fclose(outputFile);
}

int* readIntCoordsFromFile(FILE* file, int dim) {
	int* coords = new int[dim];
	for (int i = 0; i < dim; i++) {
		fscanf(file, "%d", &(coords[i]));
	}
	// eat the useless char
	fscanf(file, "%[^\n]", sBuffer);
	return coords;
}

void normalizeDataset_Int(char* inputFilePath, char* outputFilePath, int rangeStart, int rangeEnd) {
	FILE* file = fopen(inputFilePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function normalizeDataset:\nInput file open failed! Please check your input file path!\n");
	}

	vector<int*> ptList;
	ptList.reserve(N);

	vector<int> maxList;
	maxList.resize(DIM, -RAND_MAX);
	vector<int> minList;
	minList.resize(DIM, RAND_MAX);

	int* coords = NULL;

	for (int i = 0; i < N; i++) {
		coords = readIntCoordsFromFile(file, DIM);
		ptList.push_back(coords);

		for (int j = 0; j < DIM; j++) {
			if (coords[j] > maxList[j]) {
				maxList[j] = coords[j];
			}
			if (coords[j] < minList[j]) {
				minList[j] = coords[j];
			}
		}
	}

	fclose(file);

	FILE* outputFile = fopen(outputFilePath, "wt");
	if (outputFile == NULL) {
		showError(
				"Error in function normalizeDataset:\n Output file open failed! Please check your output file path!\n");
	}

	fprintf(outputFile, "%d %d\n", N, DIM);

	vector<int> MMList;
	MMList.resize(DIM);
	for (int i = 0; i < DIM; i++) {
		MMList[i] = maxList[i] - minList[i];
	}

	int rangeLength = rangeEnd - rangeStart;
	int* intCoords = new int[DIM];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < DIM; j++) {
			if (MMList[j] != 0) {
				intCoords[j] = (int) (((((ptList[i][j] - minList[j]) * 1.0) / MMList[j])
						* rangeLength)) + rangeStart;
			} else {
				intCoords[j] = rangeStart;
			}
		}

		writeIntCoordsToFile(outputFile, i + 1, intCoords, DIM);

		delete (ptList[i]);
		ptList[i] = NULL;
	}

	delete intCoords;
	ptList.clear();

	fclose(outputFile);
}

void readAndWriteDataset_Float(char* inputFilePath, char* outputFilePath, int rangeStart,
		int rangeEnd, float* (*readMethod)(FILE*, int)) {
	FILE* file = fopen(inputFilePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function readAndWriteDataset_Float:\nInput file open failed! Please check your input file path!\n");
	}

	vector<float*> ptList;
	ptList.reserve(N);

	vector<float> maxList;
	maxList.resize(DIM, -1000000000000000);
	vector<float> minList;
	minList.resize(DIM, 1000000000000000);
	float* coords = NULL;

	for (int i = 0; i < N; i++) {
		coords = readMethod(file, DIM);
		ptList.push_back(coords);

		for (int j = 0; j < DIM; j++) {
			if (coords[j] > maxList[j]) {
				maxList[j] = coords[j];
			}
			if (coords[j] < minList[j]) {
				minList[j] = coords[j];
			}
		}
	}

	fclose(file);

	FILE* outputFile = fopen(outputFilePath, "wt");
	if (outputFile == NULL) {
		showError(
				"Error in function readAndWriteDataset_Float:\n Output file open failed! Please check your output file path!\n");
	}

	fprintf(outputFile, "%d %d\n", N, DIM);

	vector<float> MMList;
	MMList.resize(DIM);
	for (int i = 0; i < DIM; i++) {
		MMList[i] = maxList[i] - minList[i];
	}
	int rangeLength = rangeEnd - rangeStart;
	int* intCoords = new int[DIM];

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < DIM; j++) {
			if (MMList[j] != 0) {
				intCoords[j] = (int) ((((ptList[i][j] - minList[j]) / MMList[j]) * rangeLength))
						+ rangeStart;
			} else {
				intCoords[j] = rangeStart;
			}
		}

		writeIntCoordsToFile(outputFile, i + 1, intCoords, DIM);

		delete (ptList[i]);
		ptList[i] = NULL;
	}

	delete intCoords;
	ptList.clear();

	fclose(outputFile);
}

float* readCoordsFromHousehold(FILE* file, int dim) {
	int temp = 0;
	fscanf(file, "%d/%d/%d;%d:%d:%d", &temp, &temp, &temp, &temp, &temp, &temp);

	float* coords = new float[dim];
	for (int i = 0; i < dim; i++) {
		fscanf(file, ";%f", &(coords[i]));
	}
	// eat the useless char
	fscanf(file, "%[^\n]", sBuffer);
	return coords;
}

void readAndWriteHousehold(char* inputFilePath, char* outputFilePath, int rangeStart,
		int rangeEnd) {
	readAndWriteDataset_Float(inputFilePath, outputFilePath, rangeStart, rangeEnd,
			&readCoordsFromHousehold);
}

float* readCoordsFromSUSY(FILE* file, int dim) {
	float tempFloat = 0.0;
	fscanf(file, "%e", &tempFloat);

	float* coords = new float[dim];
	for (int i = 0; i < dim; i++) {
		fscanf(file, ",%e", &(coords[i]));
	}

	int num = 18 - dim;
	for (int i = 0; i < num; i++) {
		fscanf(file, ",%e", &tempFloat);
	}
	// eat the useless char
	fscanf(file, "%[^\n]", sBuffer);
	return coords;
}

void readAndWriteSUSY(char* inputFilePath, char* outputFilePath, int rangeStart, int rangeEnd) {
	readAndWriteDataset_Float(inputFilePath, outputFilePath, rangeStart, rangeEnd,
			&readCoordsFromSUSY);
}

void dropDim(char* inputFilePath, char* outputFilePath, int dropNum) {
	FILE* file = fopen(inputFilePath, "rt");
	if (file == NULL) {
		showError(
				"Error in function dropDim:\nInput file open failed! Please check your input file path!\n");
	}

	FILE* outputFile = fopen(outputFilePath, "wt");
	if (outputFile == NULL) {
		showError(
				"Error in function readAndWriteDataset_Float:\n Output file open failed! Please check your output file path!\n");
	}

	int dim = DIM - dropNum;

	fprintf(outputFile, "%d %d\n", N, dim);

	vector<int*> ptList;
	ptList.reserve(N);

	int temp = 0;

	fscanf(file, "%d %d\n", &temp, &temp);

	for (int i = 0; i < N; i++) {
		fscanf(file, "%d", &temp);
		fprintf(outputFile, "%d", temp);
		for (int j = 0; j < dim; j++) {
			fscanf(file, " %d", &temp);
			fprintf(outputFile, " %d", temp);
		}

		for (int j = 0; j < dropNum; j++) {
			fscanf(file, "%d", &temp);
		}

		fscanf(file, "%[^\n]", sBuffer);
		fprintf(outputFile, "\n");
	}

	fclose(file);
	fclose(outputFile);
}

/********************************************************************/
