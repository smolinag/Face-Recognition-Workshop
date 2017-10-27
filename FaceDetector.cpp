#include <vector>
#include "FaceDetector.h"

FaceDetector::FaceDetector(
	const string &cascadePath,
	double scaleFactor,
	int    minNeighbors,
	double minSizeRatio,
	double maxSizeRatio) :
	_scaleFactor(scaleFactor), _minNeighbors(minNeighbors), _minSizeRatio(minSizeRatio), _maxSizeRatio(maxSizeRatio){
	_cascade.load(cascadePath);
}

FaceDetector::~FaceDetector() {
	_cascade.~CascadeClassifier();
}

void FaceDetector::findFacesInImage(const Mat &img, vector<Rect> &res) {
	Mat tmp;
	int width = img.size().width,
		height = img.size().height;
	Size minScaleSize = Size(_minSizeRatio  * width, _minSizeRatio  * height),
		maxScaleSize = Size(_maxSizeRatio  * width, _maxSizeRatio  * height);

	//convert the image to grayscale and normalize histogram:
	cvtColor(img, tmp, CV_BGR2GRAY);
	equalizeHist(tmp, tmp);

	//clear the vector:
	if (!res.empty()){
		res.clear();
	}

	//detect faces:
	_cascade.detectMultiScale(tmp, res, _scaleFactor, _minNeighbors, 0, minScaleSize, maxScaleSize);
}

void FaceDetector::showDetections(Mat img, vector<Rect> res, Mat &imgDet) {

	imgDet = img.clone();
	size_t nFaces = res.size();

	for (size_t i = 0; i < nFaces; i++){
		rectangle(imgDet, res[i], Scalar(255, 0, 0), 2);
	}
	cv::imshow("People Det", imgDet);
}

