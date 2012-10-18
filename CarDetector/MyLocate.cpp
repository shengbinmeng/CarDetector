
#include "stdafx.h"
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <set>
#include <ml.h> 
#include "Classify.h"
#include "Locate.h"
#include<fstream>
using namespace std;
using namespace cv;

#define MY_DEBUG 0

static void clipImage(IplImage *src, int x, int y, IplImage *dst)
{
	if(x >= 0 && y >= 0 && x <= src->width - dst->width && y <= src->height - dst->height)
	{
		CvRect rect = cvRect(x, y, dst->width, dst->height);
		cvSetImageROI(src, rect);
		cvResize(src, dst, CV_INTER_LINEAR);
	}
	else
	{
		throw new exception("Out of boundary.");
	}
}

static void clipImageScaled(IplImage *src, CvRect srcRect, IplImage *dst)
{
	if(srcRect.x >= 0 && srcRect.y >= 0 && srcRect.width <= src->width && srcRect.height <= src->height)
	{
		cvSetImageROI(src, srcRect);
		cvResize(src, dst, CV_INTER_AREA);
	}
	else
	{
		throw new exception("Out of boundary.");
	}
}

static vector<pair<int, int>> NMSAlogo(float** carV, int row, int col, int xIncre, int yIncre)
{
	vector< pair<int, int> > carCoordinates;
	set<int> delFlag;
	float min_value = 1000;
	int min_i = 0, min_j = 0;
	//取极值点
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			if(carV[i][j] < -0.2 && 
				carV[i][j] <= carV[max(i-1, 0)][j] && 
				carV[i][j] <= carV[i][max(j-1, 0)] && 
				carV[i][j] <= carV[min(i+1, row - 1)][j] &&
				carV[i][j] <= carV[i][min(j+1, col - 1)])
			{
				carCoordinates.push_back(make_pair(i * xIncre, j * yIncre));
			}

			// get the min value
			if (carV[i][j] < min_value) {
				min_value = carV[i][j];
				min_i = i;
				min_j = j;
			}
		}
	}

	//删过近的极值点
	if(carCoordinates.size() >= 1) {
		for(unsigned int i = 0; i < carCoordinates.size(); i++)
		{
			for(unsigned int j = i + 1; j < carCoordinates.size(); j++)
			{
				int xi = carCoordinates.at(i).first;
				int xj = carCoordinates.at(j).first;
				int yi = carCoordinates.at(i).second;
				int yj = carCoordinates.at(j).second;
				//因为没有竖着放的车，因此我们把y坐标值的差设的很大很大
				if(abs(xi - xj) < 80 && abs(yi - yj) < 200)
				{
					int index = (carV[xi / xIncre][yi / yIncre] > carV[xj / xIncre][yj / yIncre]) ? i : j;
					delFlag.insert(index);
					
				}
			}
		}
	} else {
		carCoordinates.push_back(make_pair(min_i * xIncre, min_j * yIncre));
	}

	//Set天然有序
	//sort(delFlag.begin(), delFlag.end());
	for( set<int>::reverse_iterator iter = delFlag.rbegin(); iter!=delFlag.rend(); iter++)
	{
		carCoordinates.erase(carCoordinates.begin() + *iter);
	}
	return carCoordinates;
}

static vector< pair<int, int> > getCarLocation(IplImage *src, int xIncr, int yIncr)
{
#if MY_DEBUG
	ofstream fout;
	fout.open("predict.txt");
#endif 

	IplImage *dst = cvCreateImage(cvSize(100, 40), src->depth, src->nChannels);

	int clipVrow = (src->width - dst->width) / xIncr + 1;
	int clipVcol = (src->height - dst->height) / yIncr + 1;
	float **clipV = new float*[clipVrow];
	for (int i = 0;  i < clipVrow;  i++)
    {
		clipV[i] = new float[clipVcol];
    }

	for(int j = 0, jj = 0; j <= src->height - dst->height; j += yIncr)
	{
		for(int i = 0, ii = 0; i <= src->width - dst->width; i += xIncr)
		{		
			try
			{
				clipImage(src, i, j, dst);
			}
			catch (exception *ex)
			{
				cout << ex->what();
			}
			Mat img(dst);
			float car = predictCar(img);
			clipV[ii][jj] = car;
#if MY_DEBUG
			fout<<"("<<i<<","<<j<<","<<car<<")  ";
#endif
			ii++;
		}
#if MY_DEBUG	
		fout<<endl;
#endif
		jj++;
	}

	vector< pair<int, int> > carCoodinates = NMSAlogo(clipV, clipVrow, clipVcol, xIncr, yIncr);
	cvReleaseImage(&dst);

#if MY_DEBUG
	fout.close();
#endif

	//cleaning up
	for (int m = 0; m < clipVrow; m++)
    {
		delete[] clipV[m];
    }
    delete[] clipV;
    clipV = NULL;

	return carCoodinates;
}

static vector< pair< pair<int, int>, int > > NMSAlgoScaled(pair<int, float>** carV, int row, int col, int xIncre, int yIncre)
{
	vector< pair< pair<int, int>, int > > carCoordinates;
	set<int> delFlag;
	float min_value = 1000;
	int min_i = 0, min_j = 0;
	//取极值点
	for(int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			if(carV[i][j].second < -0.4 && 
				carV[i][j].second <= carV[max(i-1, 0)][j].second && 
				carV[i][j].second <= carV[i][max(j-1, 0)].second && 
				carV[i][j].second <= carV[min(i+1, row - 1)][j].second &&
				carV[i][j].second <= carV[i][min(j+1, col - 1)].second)
			{
				carCoordinates.push_back(make_pair(make_pair(i * xIncre, j * yIncre), carV[i][j].first));
			}

			// get the min value
			if (carV[i][j].second < min_value) {
				min_value = carV[i][j].second;
				min_i = i;
				min_j = j;
			}
		}
	}

	//删过近的极值点，和另一个NMS区别在质心
	if(carCoordinates.size() >= 1) {
		for(int i = 0; i < carCoordinates.size(); i++)
		{
			for(int j = i + 1; j < carCoordinates.size(); j++)
			{
				int widthi = carCoordinates.at(i).second;
				int widthj = carCoordinates.at(j).second;
				int xi = carCoordinates.at(i).first.first;
				int xj = carCoordinates.at(j).first.first;
				int yi = carCoordinates.at(i).first.second;
				int yj = carCoordinates.at(j).first.second;
				int min_width = widthi < widthj ? widthi : widthj;

				// better to compare with the min width
				if (abs(xi - xj + (widthi - widthj) / 2) < min_width * 0.9 && abs(yi - yj + (widthi - widthj) / 2 * 0.4 ) < 0.4 * min_width * 0.9) {
					int index = (carV[xi / xIncre][yi / yIncre].second > carV[xj / xIncre][yj / yIncre].second) ? i : j;
					delFlag.insert(index);
				}
			}
		}
	} else {
		carCoordinates.push_back(make_pair(make_pair(min_i * xIncre, min_j * yIncre), carV[min_i][min_j].first));
	}

	//Set天然有序
	//sort(delFlag.begin(), delFlag.end());
	for( set<int>::reverse_iterator iter = delFlag.rbegin(); iter!=delFlag.rend(); iter++)
	{
		carCoordinates.erase(carCoordinates.begin() + *iter);
	}
	return carCoordinates;
}



static vector< pair< pair<int, int>, int > > getCarLocationScaled(IplImage *src, int xIncr, int yIncr, 
	float scaleLowerBound = 0.2, float scaleUpperBound = 0.9, float scaleIncr = 0.05)
{

	map<float, int> carVRow;
	
	int clipVrow = src->width * (1 - scaleLowerBound) / xIncr + 1;
	int clipVcol = src->height * (1 - scaleLowerBound) / yIncr + 1;
	pair<int, float> **clipV = new pair<int, float>*[clipVrow];
	for (int i = 0;  i < clipVrow;  i++)
    {
		clipV[i] = new pair<int, float>[clipVcol];
    }

	IplImage *dst = cvCreateImage(cvSize(100, 40), src->depth, src->nChannels);
#if MY_DEBUG
	ofstream fout;
	fout.open("predict_Scale.txt");
#endif

	for(int j = 0, jj = 0; j <= src->height * (1 - scaleLowerBound); j += yIncr)
	{
		for(int i = 0, ii = 0; i <= src->width * (1 - scaleLowerBound); i += xIncr) 
		{
			pair<int, float> minCarVPoint = make_pair(100, 10.0f);

			for(float scale = scaleLowerBound; 
				scale <= scaleUpperBound && i <= src->width * (1 - scale) && j <= src->height -  src->width * scale * 0.4; 
				scale += scaleIncr)
			{
				int scalePx = src->width * scale;
				int sw = scalePx, sh = 0.4 * scalePx;
				Mat img;
				if(sw != 100)
				{
					clipImageScaled(src,cvRect(i, j, sw, sh), dst);
				}
				else 
				{
					clipImage(src, i, j, dst);
				}
				img = Mat(dst);
		
				float car = predictCar(img);

				if(car < minCarVPoint.second)
				{
					minCarVPoint.first = scalePx;
					minCarVPoint.second = car;
				}
			}

			clipV[ii][jj] = minCarVPoint;
#if MY_DEBUG
			fout<<"("<<i<<","<<j<<","<<minCarVPoint.first<<","<<minCarVPoint.second<<")  ";
#endif			
			ii++;

		}
#if MY_DEBUG
		fout<<endl;
#endif
		jj++;

	}
#if MY_DEBUG
	cvReleaseImage(&dst);
	fout.close();
#endif

	vector< pair< pair<int, int>, int > > carCoodinatesScaled 
		= NMSAlgoScaled(clipV, clipVrow, clipVcol, xIncr, yIncr);
	//cleaning up
	for (int m = 0; m < clipVrow; m++)
    {
		delete[] clipV[m];
    }
    delete[] clipV;
    clipV = NULL;

	return carCoodinatesScaled;

}


vector<CarLocation> LocateCars(IplImage *src) {
	vector<CarLocation> locations;
	vector<pair<int, int>> cars = getCarLocation(src, 5, 5);
	for (unsigned int i = 0; i < cars.size();  i++) {
		CarLocation cl;
		cl.left = cars.at(i).first;
		cl.top = cars.at(i).second;
		cl.width = 100;
		cl.height = 40;
		locations.push_back(cl);
	}
	return locations;
}

vector<CarLocation> LocateScaledCars(IplImage *src) {
	vector<CarLocation> locations;
	vector< pair< pair<int, int>, int > > cars = getCarLocationScaled(src, 10, 10);
	for (unsigned int i = 0; i < cars.size();  i++) {
		CarLocation cl;
		cl.left = cars.at(i).first.first;
		cl.top = cars.at(i).first.second;
		cl.width = cars.at(i).second;
		cl.height = cl.width * 0.4;
		locations.push_back(cl);
	}
	return locations;
}