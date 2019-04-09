/*
 * DataGenerator.cpp
 *
 *  Created on: Sep 24, 2014
 *      Author: jhgan
 */

#include "DataGenerator.h"

#ifdef WIN
#define M_PI		3.14159265358979323846
#define random()		rand()
#endif

// Generate a random real distributed uniformly in [rangeStart,
// rangeEnd]. Input must satisfy: rangeStart <= rangeEnd. The
// granularity of generated random reals is given by RAND_MAX.
double genUniformRandom(double rangeStart, double rangeEnd) {
	if (rangeStart > rangeEnd) {
		showError(
				"Error in genUniformRandom: the rangeEnd is < the rangeStart!\n");
	}
	double r;
	r = rangeStart
			+ ((rangeEnd - rangeStart) * (double) random() / (double) RAND_MAX);
	if (r < rangeStart || r > rangeEnd) {
		showError(
				"Error in genUniformRandom: the number generated is not in the given range!\n");
	}
	return r;
}

// Generate a random real from normal distribution N(0,1).
double genGaussianRandom() {
	// Use Box-Muller transform to generate a point from normal
	// distribution.
	double x1, x2;
	do {
		x1 = genUniformRandom(0.0, 1.0);
	} while (x1 == 0); // cannot take log of 0.
	x2 = genUniformRandom(0.0, 1.0);
	double z;
	z = sqrt(-2.0 * log(x1)) * cos(2.0 * M_PI * x2);
	return z;
}

void genPtFromGaussian(vector<int>& center, double sigma, int dim,
		vector<int>& targetPlace) {
	targetPlace.clear();
	targetPlace.resize(dim, -1);
	double v = 0;

	for (int i = 0; i < dim; i++) {
		do {
			v = genGaussianRandom();
			v *= sigma;
			targetPlace[i] = center[i] + (int) (v + 0.5);
		} while (targetPlace[i] < 0);
	}
}

void genPtFromRandom(int dim, vector<int>& targetPlace, double rangeStart,
		double rangeEnd) {
	targetPlace.clear();
	targetPlace.resize(dim, -1);

	double v = 0;
	for (int i = 0; i < dim; i++) {
		v = genUniformRandom(rangeStart, rangeEnd);
		targetPlace[i] = (int) (v + 0.5);
		if (targetPlace[i] > rangeEnd) {
			targetPlace[i] = rangeEnd;
		}
	}
}

void writePointToFile(FILE* file, int id, vector<int>& coords) {
	if (file == NULL) {
		showError(
				"Error in function writePointToFile:\nThe file handler is NULL!\n");
	}

	fprintf(file, "%d", id);
	int dim = coords.size();
	for (int i = 0; i < dim; i++) {
		fprintf(file, "\t%d", coords[i]);
	}
	fprintf(file, "\n");

//	fprintf(file, "%d ", id);
//	int dim = coords.size();
//	for (int i = 0; i < dim; i++) {
//		fprintf(file, "%d ", coords[i]);
//	}
//	fprintf(file, "\n");
}

void genDatasetToFile(char* filePath, int dim, int totalNum, int clNum,
		int noiseNum, double rangeStart, double rangeEnd, double radius,
		bool move) {
	FILE* file = fopen(filePath, "wt");
	if (file == NULL) {
		showError(
				"Error in genDatasetToFile:\nFile open failed! Please check your file path!\n");
	}

	fprintf(file, "%d\t%d\n", totalNum, dim);

	vector<vector<int> > centerList;
	centerList.resize(clNum);
	for (int i = 0; i < clNum; i++) {
		genPtFromRandom(dim, centerList[i], rangeStart, rangeEnd);
	}

	int avgNum = (totalNum - noiseNum) / clNum;
	int jumpNum = avgNum / 80;
	vector<int> targetPlace;
	int id = 1;

	for (int i = 0; i < clNum; i++) {
		for (int j = 0; j < avgNum; j++) {
			genPtFromGaussian(centerList[i], radius, dim, targetPlace);
			/**********Introduce some movements for the centers.**************/

			if (j > 0 && j % jumpNum == 0) {
				printf("%d pts generated.\n", j + i * avgNum);
				fflush(stdout);
				if (move) {
//				centerList[i].swap(targetPlace);
					int sign = 1;
					for (int l = 0; l < dim; l++) {
						sign = 1;
						if (genUniformRandom(0, 1) > 0.5) {
							sign = -1;
						}
						(centerList[i])[l] = (centerList[i])[l]
								+ (int) (radius) * sign;
						if (centerList[i][l] < rangeStart) {
							centerList[i][l] = rangeStart;
						}
						if (centerList[i][l] > rangeEnd) {
							centerList[i][l] = rangeEnd;
						}
					}
				}
			}
			/**********Introduce some movements for the center.**************/

			writePointToFile(file, id, targetPlace);
			id++;
		}
	}

	noiseNum = totalNum - avgNum * clNum;

	for (int i = 0; i < noiseNum; i++) {
		genPtFromRandom(dim, targetPlace, rangeStart, rangeEnd);
		writePointToFile(file, id, targetPlace);
		id++;
	}

	fclose(file);
}

void genPtInBox(int dim, vector<int>& center, double halfSideLength,
		vector<int>& targetPlace) {
	targetPlace.clear();
	targetPlace.resize(dim, -1);

	double v = 0;
	double end = 0;
	for (int i = 0; i < dim; i++) {
		end = center[i] + halfSideLength;
		do {
			v = genUniformRandom(center[i] - halfSideLength, end);
		} while (v < 0);
		targetPlace[i] = (int) (v + 0.5);
		if (targetPlace[i] > end) {
			targetPlace[i] = end;
		}
	}
}

void genPtInBall(int dim, vector<int>& center, double radius,
		vector<int>& targetPlace) {
	double v = 0;
	double end = 0;
	double sqr_r = radius * radius;
	double sqr_dist = 0;
	double temp = 0;
	do {
		sqr_dist = 0;
		targetPlace.clear();
		targetPlace.resize(dim, -1);

		for (int i = 0; i < dim; i++) {
			end = center[i] + radius;
			do {
				v = genUniformRandom(center[i] - radius, end);
			} while (v < 0);
			targetPlace[i] = (int) (v + 0.5);
			if (targetPlace[i] > end) {
				targetPlace[i] = end;
			}
			temp = (targetPlace[i] - center[i]);
			sqr_dist += temp * temp;
		}
	} while (sqr_dist > sqr_r);
}

void genDatasetToFile(char* filePath, int dim, int totalNum, int expClNum,
		int noiseNum, double rangeStart, double rangeEnd, double boxSideLength,
		void (*genPt)(int, vector<int>&, double, vector<int>&)) {

	if (rangeEnd - rangeStart < boxSideLength) {
		showError(
				"Error in genDatasetToFile:\nThe coordinate range should be larger than the box side length!\n");
	}

	FILE* file = fopen(filePath, "wt");
	if (file == NULL) {
		showError(
				"Error in genDatasetToFile:\nFile open failed! Please check your file path!\n");
	}

	fprintf(file, "%d\t%d\n", totalNum, dim);

	// Reserve some positions for the noise points.
	totalNum = totalNum - noiseNum;

	vector<int> targetPlace;
	int id = 1;
	int curCnt = 0;
	int sign = 1;
	int restartCnt = 0;

	// For 2D visualization
//	const int ptNumInBox = 10;

	const int ptNumInBox = 100;

	const double halfSideLength = boxSideLength / 2;
	const double centerRangeStart = rangeStart + halfSideLength;
	const double centerRangeEnd = rangeEnd - halfSideLength;
	const double coeff = sqrt(dim) / 2;

	// Generate a random center.
	vector<int> curCenter;
	genPtFromRandom(dim, curCenter, centerRangeStart, centerRangeEnd);
	writePointToFile(file, id, curCenter);
	id++;
	curCnt++;

	while (id <= totalNum) {
		// Check whether jump out and restart.
		if (genUniformRandom(0, totalNum) <= expClNum) {
			// Restart.
			curCnt = 0;

			// Generate a new center.
			genPtFromRandom(dim, curCenter, centerRangeStart, centerRangeEnd);
			writePointToFile(file, id, curCenter);
			id++;
			curCnt++;

			restartCnt++;

		} else {
			// Continue.
			if (curCnt >= ptNumInBox) {
				// The current box is full. We need to make a jump.
				for (int i = 0; i < dim; i++) {
					sign = 1;
					if (genUniformRandom(0, 1) > 0.5) {
						sign = -1;
					}
					// For 2D visualization
//					curCenter[i] = curCenter[i] + (int) (halfSideLength * sign);
					curCenter[i] = curCenter[i]
							+ (int) (halfSideLength * coeff * sign);
					if (curCenter[i] > centerRangeEnd) {
						curCenter[i] = centerRangeEnd;
					} else if (curCenter[i] < centerRangeStart) {
						curCenter[i] = centerRangeStart;
					}
				}
				// Clear the current count of points in the box.
				curCnt = 0;

				writePointToFile(file, id, curCenter);
				id++;
				curCnt++;
			}
			genPt(dim, curCenter, halfSideLength, targetPlace);
			writePointToFile(file, id, targetPlace);
			id++;
			curCnt++;
		}
	}

	for (int i = 0; i < noiseNum; i++) {
		genPtFromRandom(dim, targetPlace, rangeStart, rangeEnd);
		writePointToFile(file, id, targetPlace);
		id++;
	}

	fclose(file);

	printf("The number of restart is %d.\n", restartCnt);
	fflush(stdout);
}

void genDatasetToFile(char* filePath, int dim, int totalNum) {
	int expClNum = 10;
	int noiseNum = totalNum / 10000;
	double rangeStart = 0;
	double rangeEnd = 100000;

//	int expClNum = 3;
//	int noiseNum = 100;
//	double rangeStart = 0;
//	double rangeEnd = 1500;

	// The ratio between the volume of the box and the volume of the data space should be 1 / (10^6).
//	double volumeRatio = 6.0;
//	double boxSideLength = rangeEnd / pow(10, volumeRatio / dim);

	double boxSideLength = 200;

	// For 2D visualization
//	double boxSideLength = 100;

	printf("The box side length is %.lf.\n", boxSideLength);
	fflush(stdout);

	genDatasetToFile(filePath, dim, totalNum, expClNum, noiseNum, rangeStart,
			rangeEnd, boxSideLength, &genPtInBall);
}

void genPtsInCell(FILE* file, vector<int>& cell_coords, int sideLen, int ptNum,
		int startPtID) {
	int dim = cell_coords.size();
	vector<int> rangeEnd;
	vector<int> coords;
	double temp = 0;

	coords.resize(dim, 0);
	rangeEnd.resize(dim, 0);
	for (int i = 0; i < dim; i++) {
		rangeEnd[i] = cell_coords[i] + sideLen;
	}

	for (int i = 0; i < ptNum; i++) {
		for (int j = 0; j < dim; j++) {
			temp = genUniformRandom(cell_coords[j], rangeEnd[j]);
			coords[j] = (int) temp;
		}
		writePointToFile(file, i + startPtID, coords);
	}
}

