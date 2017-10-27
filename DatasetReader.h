#ifndef DATASETREADER_H
#define DATASETREADER_H

#include <iostream>
#include "dirent.h"
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "Defintions.h"
#include <time.h>
#include <math.h>

using namespace std;
using cv::Mat;

class DatasetReader{

public:

	DatasetReader();

	~DatasetReader();

	int selectDataset();

	int getTrainingSet(size_t nSubjects, size_t nSamples, vector<SubjectInfo> &subjects);

	void generateSubjectsCollage(vector<SubjectInfo> subjects);

	void getRandomImagesForTest(size_t numImgs, vector<SubjectInfo> subjects, vector<Mat> &testImages);

private:	

	int checkExtension(string file);

	void displayMenu(vector<string> menu);

	void rescaleImage(Mat &im);

	void tile(const vector<Mat> &src, Mat &dst, int grid_x, int grid_y);	

	void findRowsAndColsForCollage(size_t numImgs, cv::Size imSize, cv::Size &collageSize, size_t &nRows, size_t &nCols);

	string mainPathStr;			//Main string for path directory
	char mainPathCh[250];		//Main char array for path directory	

	vector<string> imgFmt;		//Valid image formats
};
#endif