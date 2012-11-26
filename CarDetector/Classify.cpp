#include "stdafx.h"
#include "cvaux.h"
#include "highgui.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <ml.h> 

using namespace cv;
using namespace std;

static int svmLoaded = 0;
static SVM testSVM;
static HOGDescriptor desc(cv::Size(100, 70), cv::Size(40, 20), cv::Size(20, 10), cv::Size(20,10), 9);
static vector<float> descriptorVector;
static float testsample[3456];
float predictCar(Mat img)
{
	if (!svmLoaded) {
		testSVM.load("CarSVM.xml");
		svmLoaded = 1;
	}
	
	descriptorVector.clear();
	desc.compute(img,descriptorVector,Size(100,70),Size(0,0));

	for (int j=0;j<3456;j++)
	{
		testsample[j]=descriptorVector.at(j);
	}
	Mat sample(1,3456,CV_32FC1,testsample);

	float car = testSVM.predict(sample,true);
	/*returnDFVal?=true*/
	/*	returnDFVal – Specifies a type of the return value.
	If true and the problem is 2-class classification ,
	then the method returns the decision function value that is signed distance to the margin*/
	/*neg样本返回的是正值，pos样本返回的是负值*/

	return car;
}

float predictCar(IplImage *pImg)
{
	Mat img(pImg);
	return predictCar(img);
}
