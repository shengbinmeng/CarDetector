#pragma once

#include "cv.h"
#include <vector>
using namespace std;

typedef struct {
	int left;
	int top;
	int width;
	int height;
} CarLocation;

vector<CarLocation> LocateCars(IplImage *src);
vector<CarLocation> LocateScaledCars(IplImage *src);

vector<CarLocation> LocateCars2(IplImage *src);
vector<CarLocation> LocateScaledCars2(IplImage *src);