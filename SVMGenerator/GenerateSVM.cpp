/*
Function--generate a SVM classifier, which can predict an image contains cars or not and return signed distance to the margin.
lib file-- OpenCV2.1\vc2008\lib
executable file--OpenCV2.1\vc2008\bin
include file--OpenCV2.1\vc2008\include\opencv
dependency--cxcore210d.lib cv210d.lib highgui210d.lib cvaux210d.lib ml210d.lib (debug)
			cxcore210.lib cv210.lib highgui210.lib cvaux210.lib ml210.lib (release)
character set--Multi-Character Set

parameter--const Mat image;
		           --char *TrainPath;
return--float signed distance to the margin
global variable--CarSVM
Call--First: --- void GenerateCarSVM(char *TrainPath) --to get SVM
         Second: --- float PredictDistance(const Mat img) --  to get return
Data:2012-5-14

*****************************
Update at 2012-5-15
The generated SVM classifier is saved in a XML file.
parameter--XML file path;
Just load the XML file and can predict without train again.
Call---float PredictDistanceXML(char * xmlpath, const Mat img)

*/

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
#define   FILEILTER "*.*"

typedef pair<float,vector<float>> TrainImgDescript;
static CvSVM CarSVM ;

/* params:
winSize Size(80, 40);
blockSize Size(20, 10);
blockStride Size(10, 5);	
cellSize Size(5,5);
cellBin 9;
winStride Size(40,20);
Padding Size(0,0);
total 3528 eigenvalues
training_img's size is 100*40
*/

bool IsRoot(char *path)//Is it root zone file?
{
	char szRoot[4];
	sprintf(szRoot,"%c:\\",path[0]);
	return strcmp(path,szRoot)==0;
}

/*Get descriptors of triandata*/
void descript(char* szFile,vector <TrainImgDescript> &alldescriptors)
{
	TrainImgDescript onedescriptor;
	float trainlabel;
	vector<float>  descriptorVector ;
	IplImage *img=cvLoadImage(szFile,0);
	HOGDescriptor *desc=new HOGDescriptor(Size(80, 40), Size(20, 10), Size(10, 5), Size(5,5), 9);
	desc->compute(img,descriptorVector,Size(40,20),Size(0,0));
	if(strstr(szFile,"pos")!=NULL)	
		trainlabel=1.0;//pos	
	else//neg
		trainlabel=-1.0;//neg
	onedescriptor.first=trainlabel;
	onedescriptor.second.clear();
	onedescriptor.second.assign(descriptorVector.begin(), descriptorVector.end());
	alldescriptors.push_back(onedescriptor);//push
	free(desc);
}

/*Train to generate a SVM classifier*/
void trainSVM(vector <TrainImgDescript> &alldescriptors)
{
	//step1: prepare training set
	float labels[1050];
	static float trainingData[1050][3528];
	TrainImgDescript onedescriptor;
	vector<float> onedescriptvector;
	int i,j,fnum;
	int num=(int)alldescriptors.size();

	for(i=0;i<num;i++)
	{
		labels[i]=alldescriptors.at(i).first;		
		fnum=(int)alldescriptors.at(i).second.size();
		//cout<<fnum<<endl;//3528
		for(j=0;j<fnum;j++)
		{
			trainingData[i][j]=alldescriptors.at(i).second[j];
		}
	}	
	Mat labelsMat(1050, 1, CV_32FC1, labels); 
	Mat trainingDataMat(1050, 3528, CV_32FC1, trainingData); 
	
	//step2: set SVM parameters
	CvSVMParams params;
	// the following is all default, can be eleminated
	params.svm_type = CvSVM::C_SVC;
	params.kernel_type= CvSVM::RBF;
	params.term_crit =cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,100,FLT_EPSILON) ;

	//step3: train the SVM
	//CarSVM.train(trainingDataMat, labelsMat, Mat(),Mat(), params); 
	CarSVM.train_auto(trainingDataMat, labelsMat, Mat(), Mat(), params,4);
	//save to xml for future use
	CarSVM.save("CarSVM.xml");
}


/*Traversal in the path*/
void FindAllFiles(char *path,vector <TrainImgDescript> &alldescriptors) 
{
	char szFind[MAX_PATH];
	strcpy(szFind,path);
	if (!IsRoot(szFind))
	{
		strcat(szFind,"\\");
	}
	strcat(szFind,FILEILTER);
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(szFind,&wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do 
	{
		//printf("%s\n",wfd.cFileName);
		if (wfd.cFileName[0] == '.')
		{
			continue;
		}
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char szFile[MAX_PATH];
			if (IsRoot(path))
			{
				sprintf(szFile,"%s%s",path,wfd.cFileName);
			}
			else
			{
				sprintf(szFile,"%s\\%s",path,wfd.cFileName);
			}
			FindAllFiles(szFile,alldescriptors);
		}
		else 
		{
			char szFile[MAX_PATH];
			if (IsRoot(path))
			{
				sprintf(szFile,"%s%s",path,wfd.cFileName);
			}
			else
			{
				sprintf(szFile,"%s\\%s",path,wfd.cFileName);
			}
			descript(szFile,alldescriptors);
		}
	} while (FindNextFile(hFind,&wfd));
	FindClose(hFind);
}

void GenerateCarSVM(char *TrainPath)
{
	clock_t start,end;
	start = clock();
	vector <TrainImgDescript> alldescriptors;//all descriptors for traindata	
	FindAllFiles(TrainPath,alldescriptors);
	end = clock();	
 	printf("Descriptors Generate Time:%f seconds.\n",(double)(end-start)/CLK_TCK);
	start = clock();
	trainSVM(alldescriptors);
	end = clock();
	printf("TrainSVM Time:%f seconds.\n",(double)(end-start)/CLK_TCK);
}


int main(int argc, char* argv[])
{
	char *path;
	if(argc < 2) {
		cout<<"need path to directory of training set! - press Enter to exit .\n"<<endl;
		getchar();
		exit(1);
	} else {
		path = argv[1];
	}

	GenerateCarSVM(path);
	return 0;
}
