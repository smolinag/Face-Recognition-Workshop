#ifndef PERSONRECOGNIZER_H
#define	PERSONRECOGNIZER_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include "Defintions.h"
#include "FaceDetector.h"
#include "DatasetReader.h"

using std::vector;
using namespace cv;

class PersonRecognizer {
public:
	
	PersonRecognizer(int radius, int neighbors,	int grid_x, int grid_y, double threshold);
	
	virtual ~PersonRecognizer();

	void trainModel(vector<SubjectInfo> subjects, FaceDetector &fd);

	void testModel(vector<SubjectInfo> subjects, FaceDetector &fd, DatasetReader dr);

private:

	void recognize(Mat face, double &confidence, int &label);

	void adjustImage(Mat face, Mat &faceAdj);

	void gammaCorrection(Mat img, Mat &imgRes, const double gamma);

	Ptr<FaceRecognizer> model;
};

#endif	/* PERSONRECOGNIZER_H */