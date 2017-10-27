#include "PersonRecognizer.h"
#include "FaceDetector.h"

PersonRecognizer::PersonRecognizer(int radius, int neighbors, int grid_x, int grid_y, double threshold){

	model = createLBPHFaceRecognizer(radius, neighbors, grid_x, grid_y, threshold);
}

PersonRecognizer::~PersonRecognizer(){

}

void PersonRecognizer::trainModel(vector<SubjectInfo> subjects, FaceDetector &fd){

	vector<Mat> trainingSet;
	vector<int> trainingLbls;
	Mat faceIm, faceImGray;
	vector<Rect> faceDetections;	

	//Loop through subjects
	for (size_t i = 0; i < subjects.size(); i++){

		//Loop through subjects images
		for (size_t j = 0; j < subjects[i].trainingImages.size(); j++){

			//Find faces in images
			fd.findFacesInImage(subjects[i].trainingImages[j], faceDetections);

			//If there is a single detection identified get the image as a sample
			if (faceDetections.size() == 1){

				faceIm = subjects[i].trainingImages[j](faceDetections[0]);
				adjustImage(faceIm, faceImGray);
				trainingSet.push_back(faceImGray.clone());
				trainingLbls.push_back(i);

				imshow("Training...", faceImGray);
				waitKey(1);				
			}
		}
	}
	model->train(trainingSet, trainingLbls);
}

void PersonRecognizer::testModel(vector<SubjectInfo> subjects, FaceDetector &fd, DatasetReader dr){

	vector<Mat> testImages;
	vector<Rect> faceDetections;
	vector<string> testImNames;
	Mat testIm, faceIm;
	double confidence;
	int label;
	Point textPoint;

	//Get positive test images (images that belong to the training subjects)
	for (size_t i = 0; i < subjects.size(); i++){
		for (size_t j = 0; j < subjects[i].testImages.size(); j++){
			testIm = subjects[i].testImages[j].clone();
			testImages.push_back(testIm);
		}
	}

	//Get negative test images ( images that dont belong to the training subjects)
	dr.getRandomImagesForTest(testImages.size(), subjects, testImages);

	//Loop through all images
	size_t nIms = testImages.size();
	size_t rdmImg;
	string labelId;
	cv::Scalar labelColor;
	for (size_t i = 0; i < nIms - 1; i++){

		//Get random image from set
		rdmImg = rand() % (testImages.size() - 1);
		testIm = testImages[rdmImg].clone();
		testImages.erase(testImages.begin() + rdmImg);
		
		//Find faces in images
		fd.findFacesInImage(testIm, faceDetections);

		//If there is a single detection identified get the image as a sample
		if (faceDetections.size() == 1){
			faceIm = testIm(faceDetections[0]);
			recognize(faceIm, confidence, label);
			textPoint = cv::Point((int)(0.05*testIm.cols), (int)(0.95*testIm.rows));

			//If confidence is lower that threshold accept label 
			if (confidence <= IDENTIFICATION_THRESHOLD){
				labelId = subjects[label].name;
				labelColor = cv::Scalar(50, 200, 10);
			}
			else{//Otherwise mark as unknown
				labelId = "Unknown";
				labelColor = cv::Scalar(10, 30, 180);
			}

			//Set label in image 
			cv::putText(testIm, labelId, textPoint, cv::FONT_HERSHEY_PLAIN, 2, labelColor, 2);
			cout << "Label:" << label << " Confidence:" << confidence << endl;
			imshow("Testing subject:", testIm);
			waitKey();
		}
	}
}

void PersonRecognizer::recognize(Mat face, double &confidence, int &label){

	Mat faceImGray;
	adjustImage(face, faceImGray);
	model->predict(faceImGray, label, confidence);
	imshow("Training...", faceImGray);
	waitKey(1);
}

void PersonRecognizer::adjustImage(Mat face, Mat &faceAdj){

	Mat temp;

	//Crop image
	if (IM_CROP > 0 && IM_CROP < 1){
		size_t cropMarginRows = (int)(round((IM_CROP / 6) * (double)face.rows));
		size_t cropMarginCols = (int)(round((IM_CROP / 2) * (double)face.cols));
		Rect cropWindow = Rect(Point(cropMarginCols, cropMarginRows), Point(face.cols - cropMarginCols, face.rows - cropMarginRows));
		temp = face(cropWindow);
	}
	else{
		temp = face.clone();
	}	

	//Convert to grayscale and equalize histogram
	cvtColor(temp, temp, CV_BGR2GRAY);	

	//Gamma correction
	Mat test = temp.clone();
	gammaCorrection(test, temp, GAMMA_VALUE);

	//Resize to face standard size
	resize(temp, faceAdj, FACE_SIZE);
}

void PersonRecognizer::gammaCorrection(Mat img, Mat &imgRes, const double gamma)
{
	CV_Assert(gamma >= 0);
	//![changing-contrast-brightness-gamma-correction]
	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

	imgRes = img.clone();
	LUT(img, lookUpTable, imgRes);
}
