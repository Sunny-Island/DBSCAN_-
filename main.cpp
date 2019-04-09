/*
 * main.cpp
 *
 *  Created on: Sep 1, 2014
 *      Author: jhgan
 */

#include "DataGenerator.h"
#include "ToolFunctions.h"
#include "Evaluation.h"
#include "ExactDBSCAN.h"
#include "ApprDBSCAN.h"
#include "FileIO.h"
#include "OrgDBSCAN.h"
#include "GriDBSCAN.h"
#include "GlobalVar.h"
#include "Rtree.h"
#include "Zorder.h"
#include <ctime>
#include <string.h>
#include <fstream>

using namespace std;

/*
 * Print the usage of the program.
 */
void usage() {
	printf(
			"\n==================================================================\n");
	printf(
			"DBSCAN coded by Junhao Gan, The Chinese University of Hong Kong.\n");
	printf(
			"==================================================================\n");
	printf("Options:\n");
	printf("-algo\t{integer}\t the algorithm you choose\n");
	printf("-n\t{integer}\t the number of points\n");
	printf("-d\t{integer}\t the dimensionality of points\n");
	printf("-ds\t{string}\t the file path of the dataset\n");
	printf("-rf\t{string}\t the folder path of the clustering result\n");
//	printf("-gt\t{string}\t the file path of the ground truth\n");
	printf("-r\t{double}\t the radius Eps for DBSCAN\n");
	printf("-k\t{double}\t the core point threshold MinPts for DBSCAN\n");
	printf("-rho\t{double}\t the approximation for $rho$-Approximate DBSCAN\n");

	printf("\n");
	printf("Additional Option for our ExactDBSCAN and ApprDBSCAN:\n");
	printf(
			"-prg\t{0, 1}\t If prg = 1, the algorithm runs faster in a progressive manner but consumes more memory space. In default, prg = 0.\n");
	printf("\n");

	printf("0. Run Seed Spreader Algorithm to generate random data.\n");
	printf("   Parameter list: -algo 0 -n -d -ds\n");
	printf("\n");

	printf("1. Run our exact Algorithm.\n");
	printf("   Parameter list: -algo 1 -n -d -r -k -ds -rf [-prg 0]\n");
	printf("\n");

	printf("2. Run our approximate Algorithm.\n");
	printf("   Parameter list: -algo 2 -n -d -r -k -ds -rf -rho [-prg 0]\n");
	printf("\n");

	printf("3. Run the GriDBSCAN Algorithm.\n");
	printf("   Parameter list: -algo 3 -n -d -r -k -ds -rf\n");
	printf("\n");

	printf("4. Run the Original DBSCAN Algorithm with R-tree.\n");
	printf("   Parameter list: -algo 4 -n -d -r -k -ds -rf\n");
	printf("\n");

	printf("5. Run the Original DBSCAN Algorithm with brute-force.\n");
	printf("   Parameter list: -algo 5 -n -d -r -k -ds -rf\n");
	printf("\n");

	printf(
			"==================================================================\n");
	printf("\n");
}

/*
 *  Get the index of next unblank char from a string.
 */
int getNextChar(char *str) {
	int rtn = 0;

	// Jump over all blanks
	while (str[rtn] == ' ') {
		rtn++;
	}

	return rtn;
}

/*
 *  Get next word from a string.
 */
void getNextWord(char *str, char *word) {
	// Jump over all blanks
	while (*str == ' ') {
		str++;
	}

	while (*str != ' ' && *str != '\0') {
		*word = *str;
		str++;
		word++;
	}

	*word = '\0';
}

/*
 *  Initialization for exact DBSCAN clustering.
 *  Parameter List:
 */
void initialize_exact(bool use_NK, bool use_SpaceFilling, bool use_SpacePrior,
		bool use_Progressive) {
	USE_NK = use_NK;
	USE_SPACE_FILLING = use_SpaceFilling;
	USE_SPACE_PRIOR = use_SpacePrior;
	USE_PROGRESSIVE = use_Progressive;

	// Initialize the random integers for hashing.
	initializeRandom();

	// Compute the offset list if we don't use the space filling curve.
	if (!USE_SPACE_FILLING) {
		computeOffsetList(DIM, OFFSET_LIST);
	}
}

/*
 *  Initialization for approximate DBSCAN clustering.
 *  Parameter List:
 */
void initialize_appr(double rho, bool use_NK, bool use_SpaceFilling,
		bool use_SpacePrior, bool use_Progressive) {
	PHI = rho;
	USE_NK = use_NK;
	USE_SPACE_FILLING = use_SpaceFilling;
	USE_SPACE_PRIOR = use_SpacePrior;
	USE_PROGRESSIVE = use_Progressive;

	// Initialize the random integers for hashing.
	initializeRandom();

	// Compute the lowest level value for approximate DBSCAN.
	if (PHI > 0) {
		computeLowestLevel();
	} else {
		showError("The approximate factor rho should be greater than 0!\n");
	}
}

/*
 *  Initialization for the GriDBSCAN.
 */
void initialize_GriDBSCAN() {
	// Initialize the random integers for hashing.
	initializeRandom();
}

/*
 *  Initialize all the global variables.
 */
void initialize_GlobalVars(int n, int dim, double r, int k) {
	N = n;
	DIM = dim;
	R = r;
	K = k;
}

int main(int nargs, char **args) {

	srand(time(NULL));

	usage();

	int n = -1;
	int dim = -1;
	double r = -1;
	int k = -1;
	double rho = -1;
	bool use_NK = true;
	bool use_Progressive = false;
	bool use_SpaceFilling = true;
	bool use_SpacePrior = true;

	/*
	 *  algo is an option for which clustering algorithm we want to use:
	 *  	algo = 0:		generate dataset
	 *  	algo = 1:		Our exact algorithm
	 *  	algo = 2:		Our approximate algorithm
	 *  	algo = 3:		GriDBSCAN
	 *  	algo = 4:		original DBSCAN with R-tree
	 *  	algo = 5: 		original DBSCAN (brute force)
	 *  	algo = 6:		Gunawan's algorithm
	 */
	int algo = -100;

	// Whether write the ground truth to file or not.
	bool writeGT = false;

	int cnt = 1;
	bool failed = false;
	char *arg;
	int i;
	int w = -1;
	int evl = 0;
	char para[10];
	char ds[200] = "";
	char rf[200] = "";

	printf("The input parameters are:\n\n");

	while (cnt < nargs && !failed) {
		arg = args[cnt++];
		if (cnt == nargs) {
			failed = true;
			break;
		}

		i = getNextChar(arg);
		if (arg[i] != '-') {
			failed = true;
			break;
		}

		getNextWord(arg + i + 1, para);
		printf("%s\t", para);

		arg = args[cnt++];

		if (strcmp(para, "n") == 0) {
			n = atoi(arg);
			if (n <= 0) {
				failed = true;
				break;
			}
			printf("%d\n", n);
		} else if (strcmp(para, "d") == 0) {
			dim = atoi(arg);
			if (dim <= 0) {
				failed = true;
				break;
			}
			printf("%d\n", dim);
		} else if (strcmp(para, "algo") == 0) {
			algo = atoi(arg);
			if (algo < 0 || algo > 6) {
				failed = true;
				break;
			}
			printf("%d\n", algo);
		} else if (strcmp(para, "r") == 0) {
			r = atof(arg);
			if (r <= 0) {
				failed = true;
				break;
			}
			printf("%lf\n", r);
		} else if (strcmp(para, "k") == 0) {
			k = atoi(arg);
			if (k <= 0) {
				failed = true;
				break;
			}
			printf("%d\n", k);
		} else if (strcmp(para, "rho") == 0) {
			rho = atof(arg);
			if (rho <= 0) {
				failed = true;
				break;
			}
			printf("%lf\n", rho);
		} else if (strcmp(para, "ds") == 0) {
			getNextWord(arg, ds);
			printf("%s\n", ds);
		} else if (strcmp(para, "rf") == 0) {
			getNextWord(arg, rf);
			printf("%s\n", rf);
		} else if (strcmp(para, "w") == 0) {
			w = atoi(arg);
			if (w != 0 && w != 1) {
				failed = true;
				break;
			}
			if (w)
				writeGT = true;
			else
				writeGT = false;
			printf("%d\n", w);
		} else if (strcmp(para, "evl") == 0) {
			evl = atoi(arg);
			if (evl != 0 && evl != 1) {
				failed = true;
				break;
			}
//			if (evl)
//				writeGT = true;
//			else
//				writeGT = false;
			printf("%d\n", evl);
		} else if (strcmp(para, "nk") == 0) {
			int u = atoi(arg);
			if (u != 0 && u != 1) {
				failed = true;
				break;
			}
			if (u)
				use_NK = true;
			else
				use_NK = false;
			printf("%d\n", u);
		} else if (strcmp(para, "prg") == 0) {
			int p = atoi(arg);
			if (p != 0 && p != 1) {
				failed = true;
				break;
			}
			if (p)
				use_Progressive = true;
			else
				use_Progressive = false;
			printf("%d\n", p);
		} else if (strcmp(para, "sp") == 0) {
			int sp = atoi(arg);
			if (sp != 0 && sp != 1) {
				failed = true;
				break;
			}
			if (sp)
				use_SpacePrior = true;
			else
				use_SpacePrior = false;
			printf("%d\n", sp);
		} else {
			failed = true;
			printf("Unknown option -%s!\n\n", para);
		}

	}

	if (failed || algo == -100) {
		//usage();
		printf(
				"\n==================================================================\n");
		printf("The input parameters are incorrect! Please check them again!");
		printf(
				"\n==================================================================\n");
		return 0;
	}

	printf(
			"\n==================================================================\n");

	/*****************************************************************************/

	/*
	 *  Initialize all the global variables.
	 */
	initialize_GlobalVars(n, dim, r, k);

	if (algo == 0) {
		/*
		 *  Generate the test dataset.
		 */
		printf("Start generating data set.\n");
		fflush(stdout);
		genDatasetToFile(ds, DIM, N);
		printf("Finish generating data set.\n");
		return 0;
		/*********************************************************/
	}

	char clusterFolder[200];

	char gtFilePath[200];
	sprintf(gtFilePath, "%s_r=%.1lf_GT", ds, R);

	vector<Point*> ptList;
	ptList.reserve(N);
	Framework* framework = NULL;

	// Variables for recording time durations.
	double duration = 0;
	double start = 0;
	double end = 0;

	// Read the dataset from file.
	ptList.reserve(N);
	std::pair<int, int> min_max = readDatasetFromFile(ds, ptList);

	int MIN = min_max.first;
	int MAX = min_max.second;
	int U = (MAX > -MIN ? MAX : -MIN) << 1;
	int bitNum = ceil(log2(U)) + 1;
	int fanout = 10;

	printf("\n==================================================================\n");
	printf("Min = %d\t Max = %d\t U = %d\t bitNum = %d\n", MIN, MAX, U, bitNum);
	fflush(stdout);

	if (MIN < 0) {
		showError(
				"All the coordinate values of data points should be non-negative!\n");
	}

	// Start the timer.
	start = getCurrentTime();

	if (algo == 1 || algo == 2 || algo == 6) {
		if (algo == 1) {
			// Our exact algorithm.
			use_NK = true;
			use_SpaceFilling = true;
			initialize_exact(use_NK, use_SpaceFilling, use_SpacePrior,
					use_Progressive);
//			framework = new ExactDBSCAN(ptList, &edgeAddable_BCP);
//			framework = new ExactDBSCAN(ptList, &edgeAddable_Exact_NN);
			framework = new ExactDBSCAN(ptList, &edgeAddable_Exact);

			sprintf(clusterFolder, "%s/OurExact", rf);
		} else if (algo == 2) {
			// Our approximate algorithm.
			use_NK = true;
			use_SpaceFilling = true;
			initialize_appr(rho, use_NK, use_SpaceFilling, use_SpacePrior,
					use_Progressive);
			framework = new ApprDBSCAN(ptList);

			sprintf(clusterFolder, "%s/OurAppr", rf);
		} else if (algo == 6) {
			// Gunawan's algorithm.
			use_NK = true;
			use_SpaceFilling = false;
			use_Progressive = false;

			initialize_exact(use_NK, use_SpaceFilling, use_SpacePrior,
					use_Progressive);
			framework = new ExactDBSCAN(ptList, &edgeAddable_Exact);

			sprintf(clusterFolder, "%s/Gunawan", rf);

		}

		// Run the clustering algorithm.
		framework->clustering();
		end = getCurrentTime();
		duration = end - start;
		printf("Running time: %lf seconds.\n", duration);
		fflush(stdout);

		/*
		 *  Get back the point list.
		 */
		ptList.swap(framework->getPtList());

		/*
		 * Release the space.
		 */
		int num = OFFSET_LIST.size();
		for (int i = 0; i < num; i++) {
			OFFSET_LIST[i]->clear();
			delete OFFSET_LIST[i];
			OFFSET_LIST[i] = NULL;
		}
		OFFSET_LIST.clear();
		framework->releaseSpace();
		delete framework;
		framework = NULL;

		/****************************************************************
		 *  Evaluate the clustering result.
		 ****************************************************************/

		printf("==================================================================\n");
		printf("There are %d clusters.\n", (int) CLUSTER_LIST.size());
		fflush(stdout);


		if (evl) {
			// Write the ground truth.
			if (writeGT) {
				writeGroundTruth(CLUSTER_LIST, gtFilePath);
			} else {
				if (isCorrect(gtFilePath)) {
					printf("The clustering result is correct.\n");
				} else {
					printf(
							"There must be something wrong with your clustering results!\n");
				}
			}
		}
		printf("==================================================================\n");
		fflush(stdout);

		// Write the clustering result to files for plotting.
		printf("Start writing the clustering result to files.\n");
		fflush(stdout);
		writeClusterResultToFile(clusterFolder);
		printf("Finish writing the clustering result to files.\n");
		fflush(stdout);
	} else if (algo == 3) {

		// Initialization for GriDBSCAN.
		initialize_GriDBSCAN();

		double width = 10;

		GriDBSCAN griDBSCAN(ptList, R, K, DIM, width * R, fanout, bitNum);

		// Run the GriDBSCAN algorithm.
		griDBSCAN.clustering();

		end = getCurrentTime();
		duration = end - start;
		printf("Running time: %lf seconds.\n", duration);
		fflush(stdout);

		/*
		 *  Get back the point list.
		 */
		ptList.swap(griDBSCAN.getPtList());

		/*
		 *  Release the space.
		 */
		griDBSCAN.releaseSpace();

		/****************************************************************
		 *  Evaluate the clustering result.
		 ****************************************************************/

		printf("==================================================================\n");
		printf("total cluster number: %d\n", (int) CLUSTER_LIST.size());

		printf("There are %d clusters.\n", (int) CLUSTER_LIST.size());

		fflush(stdout);

		if (evl) {
			if (writeGT) {
				writeGroundTruth(CLUSTER_LIST, gtFilePath);
			} else {

				if (isCorrect(gtFilePath)) {
					printf("The clustering result is correct.\n");
				} else {
					printf(
							"There must be something wrong with your clustering results!\n");
				}
			}
		}

		printf("==================================================================\n");
		fflush(stdout);

		sprintf(clusterFolder, "%s/GriDBSCAN", rf);
		printf("Start writing the clustering result to files.\n");
		fflush(stdout);
		writeClusterResultToFile(clusterFolder);
		printf("Finish writing the clustering result to files.\n");

		/****************************************************************/

	} else if (algo == 4 || algo == 5) {
		// Run the original DBSCAN algorithm.
		int clusterID = 1;
		bool spacePrior = true;
//		// === Codes for debug.
//		fanout = 4;
//		// ===
		if (algo == 4) {
			// The R-tree version.
			orgDBSCAN_Rtree(ptList, R, K, clusterID, fanout, bitNum,
					spacePrior);
			printf("\n==================================================================\n");
			printf(
					"The clustering results of Original DBSCAN with R-tree:\n\n");

			sprintf(clusterFolder, "%s/OrgDBSCAN_Rtree", rf);

		} else if (algo == 5) {

			// The brute force version.
			orgDBSCAN(ptList, R, K, clusterID, spacePrior);

			printf("\n==================================================================\n");
			printf(
					"The clustering results of Original DBSCAN (brute force):\n\n");

			sprintf(clusterFolder, "%s/OrgDBSCAN_BF", rf);
		}

		end = getCurrentTime();
		duration = end - start;
		printf("Running time: %lf seconds.\n", duration);
		printf("==================================================================\n");
		printf("There are %d clusters. \n", (int) CLUSTER_LIST.size());
		fflush(stdout);

		if (evl) {
			if (writeGT) {
				writeGroundTruth(CLUSTER_LIST, gtFilePath);
			} else {

				/****************************************************************
				 *  Evaluate the clustering result.
				 ****************************************************************/
				if (isCorrect(gtFilePath)) {
					printf("The clustering result is correct.\n");
				} else {
					printf(
							"There must be something wrong with your clustering results!\n");
				}
			}
		}

		printf("==================================================================\n");
		fflush(stdout);

		printf("Start writing the clustering result to files.\n");
		fflush(stdout);
		writeClusterResultToFile(clusterFolder);
		printf("Finish writing the clustering result to files.\n");

		/****************************************************************/

	} else {
		showError("Error in main: No such an option for algo!\n");
	}

	/*
	 *  Release the space of each point.
	 */
	for (int i = 0; i < N; i++) {
		delete[] (ptList[i]->coords);
		delete (ptList[i]);
		ptList[i] = NULL;
	}
	ptList.clear();

	/*
	 *  Release the space of the global cluster list.
	 */
	releaseClusterList();

	printf("The program exits normally.\n");
	printf(
			"==================================================================\n");
	printf("\n");
	fflush(stdout);
	return 0;
}

