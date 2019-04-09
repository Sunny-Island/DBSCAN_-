/*
 * Zorder.cpp
 *
 *  Created on: Sep 17, 2014
 *      Author: jhgan
 */

#include "Zorder.h"

void encode(int* coords, Zorder& zdr, int bitNum) {

	int dim = DIM;

	// The number of integers in each row to store dim bits.
	int rowWidth = ceil(dim * 1.0 / WORD);

	// copy the coordinates
	vector<int> v;
	v.assign(coords, coords + DIM);

	// Let zdr be a vector of bitNum*rowWidth integers, each of which is initialized to 0.
	zdr.resize(bitNum * rowWidth, 0);

	int l = 0;
	int pos = 0;
	for (l = 0; l < dim; l++) {
		pos = l / WORD;
		if (l == 0) {
			zdr[pos] |= (v[l] >= 0 ? 1 : 0);
		} else
			// The highest bit is the sign of the value.
			zdr[pos] = (zdr[pos] << 1) | (v[l] >= 0 ? 1 : 0);
	}

	// Start from the lowest bits.
	for (int i = bitNum - 1; i > 0; i--) {
		for (l = 0; l < dim; l++) {
			pos = l / WORD;

			if (l == 0) {
				zdr[i * rowWidth + pos] |= (v[l] & 1);
			} else {
				zdr[i * rowWidth + pos] = (zdr[i * rowWidth + pos] << 1) | (v[l] & 1);
			}
			v[l] >>= 1;
		}
	}
}

bool compCode(const Zorder& z1, const Zorder& z2) {
	int num = z1.size();
	for (int i = 0; i < num; i++) {
		if (z1[i] != z2[i]) {
			return z1[i] < z2[i];
		}
	}
	return true;
}

void showCode(Zorder& zdr, int dim, int bitNum) {
	string rtn = "";
	int length = 0;
	int bit = 0;

	int rowWidth = ceil(dim * 1.0 / WORD);

	for (int i = 0; i < bitNum; i++) {
		rtn = "";
		// Deal with the last un-full integer.
		// The number of bits in the last integer.
		length = dim % WORD;

		for (int j = 0; j < length; j++) {
			bit = (zdr[i * rowWidth + rowWidth - 1] >> j) & 1;
			if (bit == 1) {
				rtn += "1";
			} else {
				rtn += "0";
			}
		}

		if (rowWidth > 1) {
			// The bits of the rest integers are fully used.
			for (int pos = rowWidth - 2; pos > -1; pos--) {
				for (int j = 0; j < WORD; j++) {
					bit = (zdr[i * rowWidth + pos] >> j) & 1;
					if (bit == 1) {
						rtn += "1";
					} else {
						rtn += "0";
					}
				}
			}
		}

		reverse(rtn.begin(), rtn.end());
		printf("%s \t", rtn.c_str());
	}
	printf("\n");
}

/*
 *  Compare the Z-order directly by the original coordinates.
 */
bool compZorderByCoord(const int* v1, const int* v2) {
	int pos = 0;
	int curMax = 0;
	int temp = 0;
	int dim = DIM;

	for (int i = 0; i < dim; i++) {
		temp = v1[i] ^ v2[i];
		if (curMax < temp && curMax < (temp ^ curMax)) {
			// Record the dimension where the different most significant bit lies.
			pos = i;
			curMax = temp;
		}
	}
	return v1[pos] < v2[pos];
}

bool compZorderByCoord(const vector<int>& v1, const vector<int>& v2) {
	int pos = 0;
	int curMax = 0;
	int temp = 0;
	int dim = DIM;

	for (int i = 0; i < dim; i++) {
		temp = v1[i] ^ v2[i];
		if (curMax < temp && curMax < (temp ^ curMax)) {
			// Record the dimension where the different most significant bit lies.
			pos = i;
			curMax = temp;
		}
	}
	return v1[pos] < v2[pos];
}
